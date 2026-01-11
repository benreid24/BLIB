#ifndef BLIB_RENDER_GRAPH_TASKASSETTAGS_HPP
#define BLIB_RENDER_GRAPH_TASKASSETTAGS_HPP

#include <BLIB/Render/Graph/TaskInput.hpp>
#include <BLIB/Render/Graph/TaskOutput.hpp>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
/**
 * @brief Basic struct containing the tags for a task for linking within a graph
 *
 * @ingroup Renderer
 */
struct TaskAssetTags {
    /// The type of outputs that the task can output to
    std::vector<TaskOutput> outputs;

    /// List of required inputs
    std::vector<TaskInput> requiredInputs;

    /// List of optional inputs
    std::vector<TaskInput> optionalInputs;

    /// Assets that are required during execution of the task that aren't inputs or outputs
    std::vector<std::string_view> sidecarAssets;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
