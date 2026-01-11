#include <BLIB/Render/Graph/Tasks/DeferredRenderTask.hpp>

#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
DeferredRenderTask::DeferredRenderTask()
: Task(rg::TaskIds::DeferredRenderTask) {
    assetTags.outputs.emplace_back(rg::TaskOutput(rg::AssetTags::GBuffer,
                                                  rg::TaskOutput::CreatedByTask,
                                                  {rg::TaskOutput::Shared},
                                                  rg::TaskOutput::First));

    assetTags.requiredInputs.emplace_back(rg::AssetTags::SceneInput);
}

void DeferredRenderTask::create(const rg::InitContext&) {
    // noop
}

void DeferredRenderTask::onGraphInit() {
    scene = dynamic_cast<SceneAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!scene) { throw std::runtime_error("Got invalid scene for deferred renderer"); }
}

void DeferredRenderTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got invalid output for deferred renderer"); }

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       fb->getViewport(),
                                       cfg::RenderPhases::Deferred,
                                       fb->getRenderPassId(),
                                       ctx.renderingToRenderTexture);
    scene->scene->renderOpaqueObjects(sceneCtx);
}

} // namespace rgi
} // namespace rc
} // namespace bl
