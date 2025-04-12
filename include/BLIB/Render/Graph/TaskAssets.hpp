#ifndef BLIB_RENDER_GRAPH_TASKASSETS_HPP
#define BLIB_RENDER_GRAPH_TASKASSETS_HPP

#include <BLIB/Render/Graph/TaskAssetTags.hpp>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
struct GraphAsset;

/**
 * @brief Basic struct containing the assets for a task. Corresponds to TaskAssetTags. Indices are
 *        consistent with tag indices to make finding assets simpler
 *
 * @ingroup Renderer
 */
struct TaskAssets {
    std::vector<GraphAsset*> outputs;
    std::vector<GraphAsset*> requiredInputs;
    std::vector<GraphAsset*> optionalInputs;

    /**
     * @brief Creates an empty asset set
     */
    TaskAssets();

    /**
     * @brief Sizes the inputs and sets all pointers to nullptr
     *
     * @param tags The tags for the task this belongs to
     */
    void init(const TaskAssetTags& tags);
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
