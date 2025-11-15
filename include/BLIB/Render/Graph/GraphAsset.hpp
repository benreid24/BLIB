#ifndef BLIB_RENDER_GRAPH_GRAPHASSET_HPP
#define BLIB_RENDER_GRAPH_GRAPHASSET_HPP

#include <BLIB/Render/Graph/AssetRef.hpp>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
class Task;

/**
 * @brief Wrapper around AssetRef for assets owned by a RenderGraph instance
 *
 * @ingroup Renderer
 */
struct GraphAsset {
    AssetRef asset;
    std::vector<Task*> outputtedBy;
    unsigned int firstAvailableStep;

    /**
     * @brief Creates a new graph asset
     *
     * @param asset The asset to wrap
     */
    GraphAsset(Asset* asset)
    : asset(asset)
    , firstAvailableStep(0) {
        outputtedBy.reserve(4);
    }
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
