#include <BLIB/Render/Graph/Strategies/Scene3DDeferredRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/AutoExposureAccumulateTask.hpp>
#include <BLIB/Render/Graph/Tasks/AutoExposureAdjustTask.hpp>
#include <BLIB/Render/Graph/Tasks/BloomTask.hpp>
#include <BLIB/Render/Graph/Tasks/DeferredCompositeTask.hpp>
#include <BLIB/Render/Graph/Tasks/DeferredRenderTask.hpp>
#include <BLIB/Render/Graph/Tasks/ForwardRenderTransparentTask.hpp>
#include <BLIB/Render/Graph/Tasks/Outline3DTask.hpp>
#include <BLIB/Render/Graph/Tasks/PostProcess3DTask.hpp>
#include <BLIB/Render/Graph/Tasks/SSAOTask.hpp>
#include <BLIB/Render/Graph/Tasks/ShadowMapTask.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
Scene3DDeferredRenderStrategy::Scene3DDeferredRenderStrategy()
: renderer(nullptr) {}

void Scene3DDeferredRenderStrategy::populate(rg::RenderGraph& graph) {
    graph.putTask<DeferredRenderTask>();
    graph.putTask<DeferredCompositeTask>();
    graph.putTask<ForwardRenderTransparentTask>();
    graph.putTask<rgi::PostProcess3DTask>();
    graph.putTask<rgi::ShadowMapTask>();
    graph.putTask<rgi::Outline3DTask>();
    if (renderer->getSettings().getBloomEnabled()) { graph.putTask<rgi::BloomTask>(); }
    if (renderer->getSettings().getSSAO() != Settings::SSAO::None) {
        graph.putTask<rgi::SSAOTask>();
    }
    if (renderer->getSettings().getAutoHDREnabled()) {
        graph.putTask<rgi::AutoExposureAccumulateTask>();
        graph.putTask<rgi::AutoExposureAdjustTask>();
    }
}

void Scene3DDeferredRenderStrategy::init(Renderer& renderer) {
    this->renderer = &renderer;
    subscribe(renderer.getSignalChannel());
}

void Scene3DDeferredRenderStrategy::process(const event::SettingsChanged& signal) {
    switch (signal.setting) {
    case event::SettingsChanged::Setting::BloomEnabled:
    case event::SettingsChanged::Setting::SSAO:
    case event::SettingsChanged::Setting::AutoHDR:
        invalidateGraphs();
        break;
    default:
        break;
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl
