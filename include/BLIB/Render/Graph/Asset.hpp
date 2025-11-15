#ifndef BLIB_RENDER_GRAPH_ASSET_HPP
#define BLIB_RENDER_GRAPH_ASSET_HPP

#include <BLIB/Render/Graph/AssetRef.hpp>
#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/InitContext.hpp>
#include <glm/glm.hpp>
#include <string_view>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Renderer;
class RenderTarget;

namespace rg
{
class AssetRef;
class AssetPool;
class GraphAssetPool;
class RenderGraph;
class Timeline;
class Task;
template<typename T, std::uint32_t N>
class MultiAsset;

/**
 * @brief Base class for assets in render graphs
 *
 * @ingroup Renderer
 */
class Asset {
public:
    /**
     * @brief Destroys the asset
     */
    virtual ~Asset() = default;

    /**
     * @brief Called once after the asset is added to a pool for the first time
     *
     * @param ctx The initialization context
     */
    virtual void doCreate(const InitContext& ctx) = 0;

    /**
     * @brief Prepares the asset for being an input. Child classes should insert pipeline barriers
     *        here if required
     *
     * @param context The current execution context
     */
    virtual void doPrepareForInput(const ExecutionContext& context) = 0;

    /**
     * @brief Prepares the asset for being an output. Child classes should insert pipeline barriers
     *        or start render passes here if required
     *
     * @param context The current execution context
     */
    virtual void doStartOutput(const ExecutionContext& context) = 0;

    /**
     * @brief Finalizes the asset when output is done. Child classes should insert pipeline barriers
     *        or end render passes here if required
     *
     * @param context The current execution context
     */
    virtual void doEndOutput(const ExecutionContext& context) = 0;

    /**
     * @brief Called when the observer render region changes size
     *
     * @param newSize The new size of the observer render region
     */
    virtual void onResize(glm::u32vec2 newSize);

    /**
     * @brief Called when the asset is reset before the graph is executed
     */
    virtual void onReset() {}

    /**
     * @brief Returns the tag of this asset
     */
    std::string_view getTag() const { return tag; }

    /**
     * @brief Returns the purpose of this asset
     */
    std::string_view getPurpose() const { return purpose; }

    /**
     * @brief Returns true if the asset was manually created, false if created by the factory
     */
    bool isExternal() const { return external; }

    /**
     * @brief Returns whether this asset is terminal or if it is intended to be consumed in frame
     */
    bool isTerminal() const { return terminal; }

protected:
    /**
     * @brief Initializes the asset
     *
     * @param tag The tag of the asset
     * @param terminal Whether the asset is terminal
     */
    Asset(std::string_view tag, bool terminal);

    /**
     * @brief Registers a dependency on another asset. This must be called from the constructor
     *
     * @param depTag The tag of the dependency to register
     */
    void addDependency(std::string_view depTag);

    /**
     * @brief Returns the dependency asset with the given tag. Prefer the index version for
     *        performance. Assets are ordered by the order they were added
     *
     * @param depTag The tag of the dependency to get
     * @return The dependency asset with the given tag, or nullptr if not found
     */
    Asset* getDependency(std::string_view depTag);

    /**
     * @brief Returns the dependency asset at the given index. Does not bounds check
     *
     * @param index The index of the dependency to get
     * @return The dependency asset at the given index
     */
    Asset* getDependency(unsigned int index);

    /**
     * @brief Returns whether the asset has had create() called
     */
    bool isCreated() const { return created; }

    /**
     * @brief Returns the owner that create() was called with. Only valid in doCreate
     */
    GraphAssetPool* getOwnerForLastCreate() const {
        if (owners.empty()) { return nullptr; }
        return owners.back();
    }

private:
    enum struct InputMode { Unset, Input, OutputStart, OutputEnd };

    std::string_view tag;
    std::string_view purpose;
    std::vector<std::string_view> depTags;
    std::vector<AssetRef> dependencies;
    bool created;
    unsigned int refCount;
    std::vector<GraphAssetPool*> owners;
    InputMode mode;
    bool external;
    bool terminal;

    bool create(const InitContext& ctx, GraphAssetPool* pool);
    void prepareForInput(const ExecutionContext& ctx);
    void startOutput(const ExecutionContext& ctx);
    void endOutput(const ExecutionContext& ctx);
    void reset();

    bool isOwnedBy(GraphAssetPool* pool);
    void addOwner(GraphAssetPool* pool);
    void removeOwner(GraphAssetPool* pool);

    friend class AssetRef;
    friend class AssetPool;
    friend class GraphAssetPool;
    friend class RenderGraph;
    friend class Timeline;
    friend class Task;
    template<typename T, std::uint32_t N>
    friend class MultiAsset;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline AssetRef::AssetRef(Asset* asset)
: asset(asset) {
    if (asset) { ++asset->refCount; }
}

inline AssetRef::~AssetRef() {
    if (valid()) --asset->refCount;
}

inline AssetRef::AssetRef(const AssetRef& copy)
: asset(copy.asset) {
    if (valid()) { ++asset->refCount; }
}

inline AssetRef& AssetRef::operator=(const AssetRef& copy) {
    asset = copy.asset;
    if (valid()) ++asset->refCount;
    return *this;
}

} // namespace rg
} // namespace rc
} // namespace bl

#endif
