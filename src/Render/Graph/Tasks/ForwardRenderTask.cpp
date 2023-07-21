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
    output->currentFramebuffer().beginRender(ctx.commandBuffer,
                                             output->scissor,
                                             output->clearColors,
                                             output->clearColorCount,
                                             true,
                                             output->getRenderPass().rawPass());
    vkCmdSetViewport(ctx.commandBuffer, 0, 1, &output->viewport);

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       output->viewport,
                                       output->renderPassId,
                                       ctx.renderingToRenderTexture);
    scene->scene->renderScene(sceneCtx);

    output->currentFramebuffer().finishRender(ctx.commandBuffer);
}

} // namespace rgi
} // namespace rc
} // namespace bl
