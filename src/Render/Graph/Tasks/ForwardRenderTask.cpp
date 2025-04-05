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
    output->beginRender(ctx.commandBuffer, true);

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       output->getViewport(),
                                       RenderPhase::Default,
                                       output->getRenderPassId(),
                                       ctx.renderingToRenderTexture);
    scene->scene->renderScene(sceneCtx);

    output->finishRender(ctx.commandBuffer);
}

} // namespace rgi
} // namespace rc
} // namespace bl
