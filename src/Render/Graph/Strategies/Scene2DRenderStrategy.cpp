#include <BLIB/Render/Graph/Strategies/Scene2DRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderOpaqueTask.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderTransparentTask.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
void Scene2DRenderStrategy::populate(rg::RenderGraph& graph) {
    graph.putTask<ForwardRenderOpaqueTask>();
    graph.putTask<ForwardRenderTransparentTask>();
}

} // namespace rgi
} // namespace rc
} // namespace bl
