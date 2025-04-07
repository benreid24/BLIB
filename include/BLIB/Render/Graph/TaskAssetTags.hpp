#ifndef BLIB_RENDER_GRAPH_TASKASSETTAGS_HPP
#define BLIB_RENDER_GRAPH_TASKASSETTAGS_HPP

#include <BLIB/Render/Graph/TaskInput.hpp>
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
    /// List of external assets the task can select to output to
    std::vector<std::string_view> concreteOutputs;

    /// The type of outputs that the task can create to output to
    std::vector<std::string_view> createdOutputs;

    /// List of required inputs
    std::vector<TaskInput> requiredInputs;

    /// List of optional inputs
    std::vector<TaskInput> optionalInputs;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
