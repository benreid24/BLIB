#ifndef BLIB_RENDER_GRAPH_GRAPHASSETPOOL_HPP
#define BLIB_RENDER_GRAPH_GRAPHASSETPOOL_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Graph/AssetPool.hpp>
#include <BLIB/Render/Graph/AssetRef.hpp>
#include <BLIB/Render/Graph/GraphAsset.hpp>
#include <list>
#include <string_view>
#include <unordered_map>

namespace bl
{
namespace rc
{
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
     * @param pool The parent asset pool to get assets from
     */
    GraphAssetPool(AssetPool& pool);

    /**
     * @brief Fetch an existing asset from the pool
     *
     * @param tag The asset tag to fetch
     * @return A pointer to the contained asset, or nullptr if none
     */
    GraphAsset* getAsset(std::string_view tag);

    /**
     * @brief Creates a new asset with the given tag and creator
     *
     * @param tag The asset tag
     * @param creator The Task that is creating the asset
     * @return A pointer to the new asset
     */
    GraphAsset* createAsset(std::string_view tag, Task* creator);

    /**
     * @brief Releases all held assets to allow rebuilding of the graph
     */
    void reset();

private:
    AssetPool& pool;
    std::unordered_map<std::string_view, std::list<GraphAsset>> assets;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
