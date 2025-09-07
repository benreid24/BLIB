#include <BLIB/Render/Graph/Strategies/Scene3DForwardRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/AutoExposureAccumulateTask.hpp>
#include <BLIB/Render/Graph/Tasks/AutoExposureAdjustTask.hpp>
#include <BLIB/Render/Graph/Tasks/BloomTask.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderOpaqueTask.hpp>
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
void Scene3DForwardRenderStrategy::populate(rg::RenderGraph& graph) {
    graph.putTask<ForwardRenderOpaqueTask>();
    graph.putTask<ForwardRenderTransparentTask>();
    graph.putTask<rgi::PostProcess3DTask>();
    graph.putTask<rgi::BloomTask>();
    graph.putTask<rgi::ShadowMapTask>();
    graph.putTask<rgi::Outline3DTask>();
    graph.putTask<rgi::AutoExposureAccumulateTask>();
    graph.putTask<rgi::AutoExposureAdjustTask>();
}

} // namespace rgi
} // namespace rc
} // namespace bl
