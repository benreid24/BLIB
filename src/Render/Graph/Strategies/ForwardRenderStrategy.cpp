#include <BLIB/Render/Graph/Strategies/ForwardRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderTask.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
void ForwardRenderStrategy::populate(rg::RenderGraph& graph) { graph.putTask<ForwardRenderTask>(); }

} // namespace rgi
} // namespace rc
} // namespace bl
