#include <BLIB/Render/Graph/Tasks/PostProcess3DTask.hpp>

#include <BLIB/Render/Config/PipelineIds.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentFactory.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/BloomAssets.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
PostProcess3DTask::PostProcess3DTask()
: Task(rg::TaskIds::PostProcess3DTask)
, renderer(nullptr) {
    assetTags.outputs.emplace_back(
        rg::TaskOutput({rg::AssetTags::PostFXOutput, rg::AssetTags::FinalFrameOutput},
                       {rg::TaskOutput::CreatedByTask, rg::TaskOutput::CreatedExternally},
                       {rg::TaskOutput::Exclusive, rg::TaskOutput::Shared}));
    assetTags.requiredInputs.emplace_back(rg::TaskInput(rg::AssetTags::RenderedSceneOutput));
    assetTags.optionalInputs.emplace_back(rg::AssetTags::BloomColorAttachmentPair);
    assetTags.optionalInputs.emplace_back(rg::AssetTags::AutoExposureOutput);
}

void PostProcess3DTask::create(const rg::InitContext& ctx) {
    renderer = &ctx.renderer;
    scene    = ctx.scene;

    // fetch pipeline
    pipeline = &renderer->pipelineCache().getPipeline(cfg::PipelineIds::PostProcess3D);

    // create index buffer
    indexBuffer.create(ctx.renderer, 4, 6);
    indexBuffer.indices()  = {0, 1, 3, 1, 2, 3};
    indexBuffer.vertices() = {prim::Vertex({-1.f, -1.f, 1.0f}, {0.f, 0.f}),
                              prim::Vertex({1.f, -1.f, 1.0f}, {1.f, 0.f}),
                              prim::Vertex({1.f, 1.f, 1.0f}, {1.f, 1.f}),
                              prim::Vertex({-1.f, 1.f, 1.0f}, {0.f, 1.f})};
    indexBuffer.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);
}

void PostProcess3DTask::onGraphInit() {
    FramebufferAsset* input =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }

    // init scene input descriptor set
    const auto sampler   = renderer->samplerCache().noFilterEdgeClamped();
    const auto setLayout = renderer->descriptorFactoryCache()
                               .getFactory<dsi::InputAttachmentFactory<1>>()
                               ->getDescriptorLayout();

    colorAttachmentSet.emplace(*renderer, setLayout, 1, 0);
    colorAttachmentSet.value().initAttachments(input->getAttachmentSets(), sampler);

    // init bloom blur descriptor set
    bloomAttachmentSet.emplace(*renderer, setLayout, 1, 0);
    if (assets.optionalInputs[0]) {
        auto& bloomBlur =
            dynamic_cast<BloomColorAttachmentPairAsset&>(assets.optionalInputs[0]->asset.get());
        bloomAttachmentSet.value().initAttachments(bloomBlur.get(0).getAttachmentSets(), sampler);
    }
    else {
        vk::SemanticTextureFormat formats[] = {vk::SemanticTextureFormat::SFloatR16G16B16A16};
        VkImageUsageFlags usages[] = {VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT};
        dummyBloomBuffer.emplace();
        dummyBloomBuffer.value().create(*renderer, 1, {32, 32}, formats, usages);
        dummyBloomBuffer.value().getBuffer(0).clearAndTransition(
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {});
        bloomAttachmentSet.value().initAttachments(&dummyBloomBuffer.value().attachmentSet(),
                                                   sampler);
    }
}

void PostProcess3DTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got bad output"); }

    pipeline->bind(ctx.commandBuffer, fb->getRenderPassId(), 0);
    colorAttachmentSet->bind(ctx.commandBuffer,
                             VK_PIPELINE_BIND_POINT_GRAPHICS,
                             pipeline->pipelineLayout().rawLayout(),
                             0);
    bloomAttachmentSet->bind(ctx.commandBuffer,
                             VK_PIPELINE_BIND_POINT_GRAPHICS,
                             pipeline->pipelineLayout().rawLayout(),
                             1);
    renderer->getGlobalDescriptorData().bindDescriptors(ctx.commandBuffer,
                                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                        pipeline->pipelineLayout().rawLayout(),
                                                        2,
                                                        ctx.renderingToRenderTexture);
    indexBuffer.bindAndDraw(ctx.commandBuffer);
}

void PostProcess3DTask::update(float) {}

} // namespace rgi
} // namespace rc
} // namespace bl
