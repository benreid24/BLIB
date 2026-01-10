#include <BLIB/Render/Graph/Tasks/SSAOTask.hpp>

#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>
#include <BLIB/Render/Descriptors/Builtin/SSAOInstance.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/GBufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/SSAOAsset.hpp>
#include <BLIB/Render/Graph/Purpose.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Util/Random.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
SSAOTask::SSAOTask()
: Task(rg::TaskIds::SSAOTask)
, renderer(nullptr) {
    assetTags.outputs.emplace_back(rg::TaskOutput(rg::AssetTags::SSAOBuffer,
                                                  {rg::TaskOutput::CreatedByTask},
                                                  {rg::TaskOutput::Exclusive})
                                       .withPurpose(Purpose::SSAOBuffer));
    assetTags.requiredInputs.emplace_back(rg::TaskInput(rg::AssetTags::GBuffer));
    assetTags.sidecarAssets.emplace_back(rg::AssetTags::SSAOBuffer);
}

void SSAOTask::create(const rg::InitContext& ctx) {
    renderer = &ctx.renderer;

    vk::Pipeline& gen  = ctx.renderer.pipelineCache().getPipeline(cfg::PipelineIds::SSAOGen);
    vk::Pipeline& blur = ctx.renderer.pipelineCache().getPipeline(cfg::PipelineIds::SSAOBlur);
    ds::DescriptorSetInstanceCache& sets = *ctx.target.getDescriptorSetCache(ctx.scene);

    genPipeline.init(&gen, sets);
    blurPipeline.init(&blur, sets);

    fullscreenRect.create(ctx.renderer, 4, 6);
    fullscreenRect.indices()  = {0, 1, 3, 1, 2, 3};
    fullscreenRect.vertices() = {prim::Vertex3D({-1.f, -1.f, 1.0f}, {0.f, 0.f}),
                                 prim::Vertex3D({1.f, -1.f, 1.0f}, {1.f, 0.f}),
                                 prim::Vertex3D({1.f, 1.f, 1.0f}, {1.f, 1.f}),
                                 prim::Vertex3D({-1.f, 1.f, 1.0f}, {0.f, 1.f})};
    fullscreenRect.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);
}

void SSAOTask::onGraphInit() {
    FramebufferAsset* gbuf =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    genSurface  = dynamic_cast<SSAOAsset*>(&assets.sidecarAssets[0]->asset.get());
    blurSurface = dynamic_cast<SSAOAsset*>(&assets.outputs[0]->asset.get());
    if (!genSurface || !blurSurface || !gbuf) {
        throw std::runtime_error("Bad asset initialization");
    }

    VkSampler sampler = renderer->samplerCache().noFilterEdgeClamped();
    dsi::InputAttachmentInstance* gbufInputSet =
        genPipeline.getDescriptorSet<dsi::InputAttachmentInstance>(0);
    gbufInputSet->initAttachments(gbuf->getAttachmentSets(), sampler);

    dsi::InputAttachmentInstance* blurInputSet =
        blurPipeline.getDescriptorSet<dsi::InputAttachmentInstance>(0);
    blurInputSet->initAttachments(genSurface->getAttachmentSets(), sampler);
}

void SSAOTask::execute(const rg::ExecutionContext& ctx, rg::Asset*) {
    // we do not remove the task on disable so need to check here
    if (renderer->getSettings().getSSAO() == Settings::SSAO::None) { return; }

    // gen ssao buffer
    scene::SceneRenderContext genCtx(ctx.commandBuffer,
                                     ctx.observerIndex,
                                     genSurface->getViewport(),
                                     cfg::RenderPhases::PostProcess,
                                     genSurface->getRenderPassId(),
                                     false);
    genSurface->beginRender(ctx.commandBuffer, true);
    genPipeline.bind(genCtx);
    fullscreenRect.bindAndDraw(ctx.commandBuffer);
    genSurface->finishRender(ctx.commandBuffer);

    // blur ssao buffer
    scene::SceneRenderContext blurCtx(ctx.commandBuffer,
                                      ctx.observerIndex,
                                      blurSurface->getViewport(),
                                      cfg::RenderPhases::PostProcess,
                                      blurSurface->getRenderPassId(),
                                      false);
    blurSurface->beginRender(ctx.commandBuffer, true);
    blurPipeline.bind(genCtx);
    fullscreenRect.bindAndDraw(ctx.commandBuffer);
    blurSurface->finishRender(ctx.commandBuffer);
}

} // namespace rgi
} // namespace rc
} // namespace bl
