#include <BLIB/Render/Graph/Tasks/ForwardRenderTransparentTask.hpp>

#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ForwardRenderTransparentTask::ForwardRenderTransparentTask()
: Task(rg::TaskIds::ForwardRenderTransparentTask) {
    assetTags.outputs.emplace_back(
        rg::TaskOutput({rg::AssetTags::RenderedSceneOutput, rg::AssetTags::FinalFrameOutput},
                       {rg::TaskOutput::CreatedByTask, rg::TaskOutput::CreatedExternally},
                       {rg::TaskOutput::Shared},
                       rg::TaskOutput::Middle,
                       {rg::TaskIds::ForwardRenderOpaqueTask, rg::TaskIds::DeferredCompositeTask}));

    assetTags.requiredInputs.emplace_back(rg::AssetTags::SceneInput);
    assetTags.optionalInputs.emplace_back(rg::AssetTags::ShadowMaps);
}

void ForwardRenderTransparentTask::create(const rg::InitContext&) {
    // noop
}

void ForwardRenderTransparentTask::onGraphInit() {
    scene = dynamic_cast<SceneAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!scene) { throw std::runtime_error("Got invalid scene for forward renderer"); }
}

void ForwardRenderTransparentTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got invalid output for forward renderer"); }

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       fb->getViewport(),
                                       cfg::RenderPhases::Forward,
                                       fb->getRenderPassId(),
                                       ctx.renderingToRenderTexture);
    scene->scene->renderTransparentObjects(sceneCtx);
}

} // namespace rgi
} // namespace rc
} // namespace bl
