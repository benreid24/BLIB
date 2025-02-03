#include <BLIB/Render/Graph/Tasks/ForwardRenderTask.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/BatchedScene.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ForwardRenderTask::ForwardRenderTask() {
    assetTags.concreteOutputs.emplace_back(rg::AssetTags::FinalFrameOutput);
    assetTags.createdOutput = rg::AssetTags::RenderedSceneOutput;
    assetTags.requiredInputs.emplace_back(rg::AssetTags::SceneObjectsInput);
    // TODO - allow scene hook to find task and add more tags (ie shadow maps)?
}

void ForwardRenderTask::create(engine::Engine&, Renderer&) {
    // noop
}

void ForwardRenderTask::onGraphInit() {
    output = dynamic_cast<FramebufferAsset*>(&assets.output->asset.get());
    if (!output) { throw std::runtime_error("Got invalid output for forward renderer"); }

    scene = dynamic_cast<SceneAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!scene) { throw std::runtime_error("Got invalid scene for forward renderer"); }
}

void ForwardRenderTask::execute(const rg::ExecutionContext& ctx) {
    const bool doRenderPass = !ctx.isFinalStep && !ctx.renderingToRenderTexture;

    if (doRenderPass) {
        output->currentFramebuffer().beginRender(ctx.commandBuffer,
                                                 output->scissor,
                                                 output->clearColors,
                                                 output->clearColorCount,
                                                 ctx.renderingToRenderTexture,
                                                 output->getRenderPass().rawPass());
    }
    else {
        VkClearAttachment attachments[2]{};
        attachments[0].aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
        attachments[0].colorAttachment = 0;
        attachments[0].clearValue      = output->clearColors[0];
        attachments[1].aspectMask      = VK_IMAGE_ASPECT_DEPTH_BIT;
        attachments[1].clearValue      = output->clearColors[1];

        VkClearRect rects[2]{};
        rects[0].rect           = output->scissor;
        rects[0].baseArrayLayer = 0;
        rects[0].layerCount     = 1;
        rects[1]                = rects[0];

        vkCmdClearAttachments(ctx.commandBuffer, 2, attachments, 2, rects);
    }

    vkCmdSetScissor(ctx.commandBuffer, 0, 1, &output->scissor);
    vkCmdSetViewport(ctx.commandBuffer, 0, 1, &output->viewport);

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       output->viewport,
                                       RenderPhase::Default,
                                       output->renderPassId,
                                       ctx.renderingToRenderTexture);
    scene->scene->renderScene(sceneCtx);

    if (doRenderPass) { output->currentFramebuffer().finishRender(ctx.commandBuffer); }
}

} // namespace rgi
} // namespace rc
} // namespace bl
