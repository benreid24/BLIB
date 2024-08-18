#ifndef BLIB_RENDER_GRAPH_ASSET_HPP
#define BLIB_RENDER_GRAPH_ASSET_HPP

#include <BLIB/Render/Graph/ExecutionContext.hpp>
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
     * @param engine The engine instance
     * @param renderer The renderer instance
     * @param observer The observer that the asset belongs to
     */
    virtual void doCreate(engine::Engine& engine, Renderer& renderer, RenderTarget* observer) = 0;

    /**
     * @brief Prepares the asset for being an input. Child classes should insert pipeline barriers
     *        here if required
     *
     * @param context The current execution context
     */
    virtual void doPrepareForInput(const ExecutionContext& context) = 0;

    /**
     * @brief Prepares the asset for being an output. Child classes should insert pipeline barriers
     *        here if required
     *
     * @param context The current execution context
     */
    virtual void doPrepareForOutput(const ExecutionContext& context) = 0;

    /**
     * @brief Returns the tag of this asset
     */
    constexpr std::string_view getTag() const { return tag; }

    /**
     * @brief Returns true if the asset was manually created, false if created by the factory
     */
    constexpr bool isExternal() const { return external; }

protected:
    /**
     * @brief Initializes the asset
     *
     * @param tag The tag of the asset
     */
    Asset(std::string_view tag);

private:
    enum struct InputMode { Unset, Input, Output };

    std::string_view tag;
    bool created;
    unsigned int refCount;
    std::vector<GraphAssetPool*> owners;
    InputMode mode;
    bool external;

    void create(engine::Engine& engine, Renderer& renderer, RenderTarget* observer);
    void prepareForInput(const ExecutionContext& ctx);
    void prepareForOutput(const ExecutionContext& ctx);

    bool isOwnedBy(GraphAssetPool* pool);
    void addOwner(GraphAssetPool* pool);
    void removeOwner(GraphAssetPool* pool);

    friend class AssetRef;
    friend class AssetPool;
    friend class RenderGraph;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
