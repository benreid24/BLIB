#include <BLIB/Render/Graph/Tasks/ForwardRenderOpaqueTask.hpp>

#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/BatchedScene.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ForwardRenderOpaqueTask::ForwardRenderOpaqueTask() {
    assetTags.outputs.emplace_back(rg::TaskOutput({rg::AssetTags::RenderedSceneOutputHDR,
                                                   rg::AssetTags::RenderedSceneOutput,
                                                   rg::AssetTags::FinalFrameOutput},
                                                  {rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedExternally},
                                                  {rg::TaskOutput::Shared},
                                                  rg::TaskOutput::First));

    assetTags.requiredInputs.emplace_back(rg::AssetTags::SceneInput);
    assetTags.optionalInputs.emplace_back(rg::AssetTags::ShadowMaps);
}

void ForwardRenderOpaqueTask::create(engine::Engine&, Renderer&, Scene*) {
    // noop
}

void ForwardRenderOpaqueTask::onGraphInit() {
    scene = dynamic_cast<SceneAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!scene) { throw std::runtime_error("Got invalid scene for forward renderer"); }
}

void ForwardRenderOpaqueTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got invalid output for forward renderer"); }

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       fb->getViewport(),
                                       cfg::RenderPhases::Forward,
                                       fb->getRenderPassId(),
                                       ctx.renderingToRenderTexture);
    scene->scene->renderOpaqueObjects(sceneCtx);
}

} // namespace rgi
} // namespace rc
} // namespace bl
