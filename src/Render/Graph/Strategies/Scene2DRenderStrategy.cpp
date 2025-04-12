#include <BLIB/Render/Graph/Strategies/Scene2DRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderTask.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
// TODO - split transparent + opaque tasks
void Scene2DRenderStrategy::populate(rg::RenderGraph& graph) { graph.putTask<ForwardRenderTask>(); }

} // namespace rgi
} // namespace rc
} // namespace bl
