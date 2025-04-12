#include <BLIB/Render/Graph/Tasks/RenderOverlayTask.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/BatchedScene.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
RenderOverlayTask::RenderOverlayTask(unsigned int* observerIndexOverride)
: observerIndexOverride(observerIndexOverride) {
    assetTags.outputs.emplace_back(rg::TaskOutput({rg::AssetTags::RenderedSceneOutputHDR,
                                                   rg::AssetTags::RenderedSceneOutput,
                                                   rg::AssetTags::FinalFrameOutput},
                                                  {rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedExternally},
                                                  {rg::TaskOutput::Shared},
                                                  rg::TaskOutput::Last));

    assetTags.requiredInputs.emplace_back(
        rg::TaskInput({rg::AssetTags::OverlayInput, rg::AssetTags::SceneInput}));
}

void RenderOverlayTask::create(engine::Engine&, Renderer&, Scene*) {
    // noop
}

void RenderOverlayTask::onGraphInit() {
    scene = dynamic_cast<SceneAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!scene) { throw std::runtime_error("Got invalid scene for forward renderer"); }
}

void RenderOverlayTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got invalid output for forward renderer"); }

    VkClearValue clearColor{};
    clearColor.depthStencil = {1.f, 0};

    VkClearAttachment attachment{};
    attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    attachment.clearValue = clearColor;

    VkClearRect rect{};
    rect.rect           = fb->getScissor();
    rect.baseArrayLayer = 0;
    rect.layerCount     = 1;

    vkCmdClearAttachments(ctx.commandBuffer, 1, &attachment, 1, &rect);

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       observerIndexOverride ? *observerIndexOverride :
                                                               ctx.observerIndex,
                                       fb->getViewport(),
                                       RenderPhase::Overlay,
                                       fb->getRenderPassId(),
                                       ctx.renderingToRenderTexture);
    scene->scene->renderScene(sceneCtx);
}

} // namespace rgi
} // namespace rc
} // namespace bl
