#include <BLIB/Render/Graph/Strategies/Scene3DForwardRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/BloomTask.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderOpaqueTask.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderTransparentTask.hpp>
#include <BLIB/Render/Graph/Tasks/PostProcess3DTask.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
void Scene3DForwardRenderStrategy::populate(rg::RenderGraph& graph) {
    graph.putTask<ForwardRenderOpaqueTask>();
    graph.putTask<ForwardRenderTransparentTask>();
    graph.putTask<rgi::PostProcess3DTask>();
    graph.putTask<rgi::BloomTask>();
}

} // namespace rgi
} // namespace rc
} // namespace bl
