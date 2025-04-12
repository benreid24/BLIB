#include <BLIB/Render/Graph/Tasks/ForwardRenderTransparentTask.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/BatchedScene.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ForwardRenderTransparentTask::ForwardRenderTransparentTask() {
    assetTags.outputs.emplace_back(rg::TaskOutput({rg::AssetTags::RenderedSceneOutputHDR,
                                                   rg::AssetTags::RenderedSceneOutput,
                                                   rg::AssetTags::FinalFrameOutput},
                                                  {rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedExternally},
                                                  {rg::TaskOutput::Shared},
                                                  rg::TaskOutput::Middle));

    assetTags.requiredInputs.emplace_back(rg::AssetTags::SceneInput);
}

void ForwardRenderTransparentTask::create(engine::Engine&, Renderer&, Scene*) {
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
                                       RenderPhase::Default,
                                       fb->getRenderPassId(),
                                       ctx.renderingToRenderTexture);
    scene->scene->renderTransparentObjects(sceneCtx);
}

} // namespace rgi
} // namespace rc
} // namespace bl
