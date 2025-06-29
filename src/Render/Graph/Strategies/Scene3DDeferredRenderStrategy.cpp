#include <BLIB/Render/Graph/Strategies/Scene3DDeferredRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/BloomTask.hpp>
#include <BLIB/Render/Graph/Tasks/DeferredCompositeTask.hpp>
#include <BLIB/Render/Graph/Tasks/DeferredRenderTask.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderTransparentTask.hpp>
#include <BLIB/Render/Graph/Tasks/Outline3DTask.hpp>
#include <BLIB/Render/Graph/Tasks/PostProcess3DTask.hpp>
#include <BLIB/Render/Graph/Tasks/ShadowMapTask.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
void Scene3DDeferredRenderStrategy::populate(rg::RenderGraph& graph) {
    graph.putTask<DeferredRenderTask>();
    graph.putTask<DeferredCompositeTask>();
    graph.putTask<ForwardRenderTransparentTask>();
    graph.putTask<rgi::PostProcess3DTask>();
    graph.putTask<rgi::BloomTask>();
    graph.putTask<rgi::ShadowMapTask>();
    graph.putTask<rgi::Outline3DTask>();
}

} // namespace rgi
} // namespace rc
} // namespace bl
