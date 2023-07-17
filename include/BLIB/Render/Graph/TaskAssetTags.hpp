#ifndef BLIB_RENDER_GRAPH_TASKASSETTAGS_HPP
#define BLIB_RENDER_GRAPH_TASKASSETTAGS_HPP

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
    std::vector<std::string_view> concreteOutputs;
    std::string_view createdOutput;
    std::vector<std::string_view> requiredInputs;
    std::vector<std::string_view> optionalInputs;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
