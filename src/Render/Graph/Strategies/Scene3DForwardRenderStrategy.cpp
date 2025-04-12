#include <BLIB/Render/Graph/Strategies/Scene3DForwardRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/BloomTask.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderTask.hpp>
#include <BLIB/Render/Graph/Tasks/PostProcess3DTask.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
// TODO - split transparent + opaque tasks
void Scene3DForwardRenderStrategy::populate(rg::RenderGraph& graph) {
    graph.putTask<ForwardRenderTask>();
    graph.putTask<rgi::PostProcess3DTask>();
    graph.putTask<rgi::BloomTask>();
}

} // namespace rgi
} // namespace rc
} // namespace bl
