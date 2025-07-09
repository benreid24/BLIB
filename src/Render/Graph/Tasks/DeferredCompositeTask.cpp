#include <BLIB/Render/Graph/Tasks/DeferredCompositeTask.hpp>

#include <BLIB/Render/Config/PipelineIds.hpp>
#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentFactory.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/BloomAssets.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
DeferredCompositeTask::DeferredCompositeTask()
: Task(rg::TaskIds::DeferredCompositeTask)
, renderer(nullptr) {
    assetTags.outputs.emplace_back(rg::TaskOutput({rg::AssetTags::RenderedSceneOutputHDR,
                                                   rg::AssetTags::RenderedSceneOutput,
                                                   rg::AssetTags::FinalFrameOutput},
                                                  {rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedByTask,
                                                   rg::TaskOutput::CreatedExternally},
                                                  {rg::TaskOutput::Shared},
                                                  rg::TaskOutput::First));
    assetTags.requiredInputs.emplace_back(
        rg::TaskInput({rg::AssetTags::GBufferHDR, rg::AssetTags::GBuffer}));
}

void DeferredCompositeTask::create(engine::Engine&, Renderer& r, Scene* s) {
    renderer = &r;
    scene    = s;

    // fetch pipeline
    pipeline = &renderer->pipelineCache().getPipeline(cfg::PipelineIds::DeferredLightVolume);

    // create index buffer
    indexBuffer.create(r.vulkanState(), 4, 6);
    indexBuffer.indices()  = {0, 1, 3, 1, 2, 3};
    indexBuffer.vertices() = {prim::Vertex({-1.f, -1.f, 1.0f}, {0.f, 0.f}),
                              prim::Vertex({1.f, -1.f, 1.0f}, {1.f, 0.f}),
                              prim::Vertex({1.f, 1.f, 1.0f}, {1.f, 1.f}),
                              prim::Vertex({-1.f, 1.f, 1.0f}, {0.f, 1.f})};
    indexBuffer.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);

    // TODO - create unit sphere
}

void DeferredCompositeTask::onGraphInit() {
    FramebufferAsset* input =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }

    const auto sampler   = renderer->vulkanState().samplerCache.noFilterEdgeClamped();
    const auto setLayout = renderer->descriptorFactoryCache()
                               .getOrCreateFactory<ds::InputAttachmentFactory<4>>()
                               ->getDescriptorLayout();

    sceneDescriptor = &scene->getDescriptorSet<ds::Scene3DInstance>();

    gbufferDescriptor.emplace(renderer->vulkanState(), setLayout, 4, 0);
    gbufferDescriptor.value().initAttachments(input->getAttachmentSets(), sampler);
}

void DeferredCompositeTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* input =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got bad output"); }

    pipeline->bind(ctx.commandBuffer, fb->getRenderPassId(), 0);
    gbufferDescriptor->bind(ctx.commandBuffer, pipeline->pipelineLayout().rawLayout(), 0);
    sceneDescriptor->bind(
        ctx.commandBuffer, pipeline->pipelineLayout().rawLayout(), 1, ctx.observerIndex);
    indexBuffer.bindAndDraw(ctx.commandBuffer);

    // TODO - issue draw commands for other lights
}

} // namespace rgi
} // namespace rc
} // namespace bl
