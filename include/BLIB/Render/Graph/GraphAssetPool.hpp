#ifndef BLIB_RENDER_GRAPH_GRAPHASSETPOOL_HPP
#define BLIB_RENDER_GRAPH_GRAPHASSETPOOL_HPP

#include <BLIB/Render/Events/GraphEvents.hpp>
#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Graph/AssetPool.hpp>
#include <BLIB/Render/Graph/AssetRef.hpp>
#include <BLIB/Render/Graph/GraphAsset.hpp>
#include <BLIB/Render/Graph/TaskOutput.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <list>
#include <string_view>
#include <unordered_map>

namespace bl
{
namespace rc
{
class Renderer;

namespace rg
{
/**
 * @brief Asset pool for individual RenderGraph instances that exist per scene per observer
 *
 * @ingroup Renderer
 */
class GraphAssetPool {
public:
    /**
     * @brief Creates the pool
     *
     * @param renderer The renderer that owns this pool
     * @param pool The parent asset pool to get assets from
     * @param owner The render target that owns the pool
     * @param scene The scene that the pool is for
     */
    GraphAssetPool(Renderer& renderer, AssetPool& pool, RenderTarget* owner, Scene* scene);

    /**
     * @brief Fetch an existing asset from the pool to be used as an output
     *
     * @param tag The asset tag to fetch
     * @param task The task that will be outputting to the asset
     * @return A pointer to the contained asset, or nullptr if none
     */
    GraphAsset* getAssetForOutput(std::string_view tag, Task* task);

    /**
     * @brief Fetch an existing asset from the pool to be used as a shared output
     *
     * @param tag The tag of the asset being requested
     * @param sharedWith The tags of the tasks that this asset may be shared with
     * @return A pointer to the contained asset, or nullptr if none
     */
    GraphAsset* getAssetForSharedOutput(std::string_view tag,
                                        const decltype(TaskOutput::sharedWith)& sharedWith);

    /**
     * @brief Fetch an existing asset from the pool to be used as an input. Only use for external
     *        assets. Do not call after calls to createAsset
     *
     * @param tag The asset tag to fetch
     * @return A pointer to the contained asset, or nullptr if none
     */
    GraphAsset* getAssetForInput(std::string_view tag);

    /**
     * @brief Creates a new asset with the given tag and creator
     *
     * @param tag The asset tag
     * @return A pointer to the new asset
     */
    GraphAsset* createAsset(std::string_view tag);

    /**
     * @brief Gets or creates an asset to be used by another asset
     *
     * @param tag The tag of the asset to get or create
     * @return A ref to the created asset. May be invalid
     */
    AssetRef getAssetForAsset(std::string_view tag);

    /**
     * @brief Fetches the swap frame asset from the pool
     */
    GraphAsset* getFinalOutput();

    /**
     * @brief Releases all held assets to allow rebuilding of the graph
     */
    void reset();

private:
    RenderTarget* owner;
    Scene* scene;
    AssetPool& pool;
    std::unordered_map<std::string_view, std::list<GraphAsset>> assets;
    sig::Emitter<event::SceneGraphAssetCreated> emitter;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
