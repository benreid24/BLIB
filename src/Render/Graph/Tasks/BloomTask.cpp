#include <BLIB/Render/Graph/Tasks/BloomTask.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
namespace
{
struct UniformPayload {
    std::uint32_t horizontal;
    float lightThreshold;
    std::uint32_t filterSize;
    float filters[Settings::MaxBloomFilterSize];

    UniformPayload(Renderer& r)
    : horizontal(0)
    , lightThreshold(r.getSettings().getBloomHighlightThreshold())
    , filterSize(r.getSettings().getBloomFilterSize()) {
        for (std::uint32_t i = 0; i < filterSize; ++i) {
            filters[i] = r.getSettings().getBloomFilters()[i];
        }
    }
};
} // namespace

BloomTask::BloomTask() {
    assetTags.requiredInputs.emplace_back(rg::TaskInput(rg::AssetTags::RenderedSceneOutputHDR));
    assetTags.outputs.emplace_back(rg::TaskOutput(rg::AssetTags::BloomColorAttachmentPair,
                                                  rg::TaskOutput::CreatedByTask,
                                                  rg::TaskOutput::Exclusive));
}

void BloomTask::create(engine::Engine&, Renderer& r, Scene*) {
    renderer = &r;

    filterHighlightPipeline =
        &renderer->pipelineCache().getPipeline(Config::PipelineIds::BloomHighlightFilter);
    blurPipeline = &renderer->pipelineCache().getPipeline(Config::PipelineIds::BloomBlur);

    indexBuffer.create(renderer->vulkanState(), 4, 6);
    indexBuffer.indices()  = {0, 1, 3, 1, 2, 3};
    indexBuffer.vertices() = {prim::Vertex({-1.f, -1.f, 1.0f}, {0.f, 0.f}),
                              prim::Vertex({1.f, -1.f, 1.0f}, {1.f, 0.f}),
                              prim::Vertex({1.f, 1.f, 1.0f}, {1.f, 1.f}),
                              prim::Vertex({-1.f, 1.f, 1.0f}, {0.f, 1.f})};
    indexBuffer.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);
}

void BloomTask::onGraphInit() {
    input = dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }
    output = dynamic_cast<BloomColorAttachmentPairAsset*>(&assets.outputs[0]->asset.get());
    if (!output) { throw std::runtime_error("Got bad output"); }

    const auto sampler   = renderer->vulkanState().samplerCache.noFilterEdgeClamped();
    const auto setLayout = renderer->descriptorFactoryCache()
                               .getFactoryThatMakes<ds::ColorAttachmentInstance>()
                               ->getDescriptorLayout();

    inputAttachmentDescriptor.emplace(renderer->vulkanState(), setLayout);
    inputAttachmentDescriptor.value().initAttachments(input->getAttachmentSets(), 0, sampler);

    output1Descriptor.emplace(renderer->vulkanState(), setLayout);
    output1Descriptor.value().initAttachments(output->get(0).getAttachmentSets(), 0, sampler);

    output2Descriptor.emplace(renderer->vulkanState(), setLayout);
    output2Descriptor.value().initAttachments(output->get(1).getAttachmentSets(), 0, sampler);
}

void BloomTask::execute(const rg::ExecutionContext& ctx, rg::Asset*) {
    BloomColorAttachmentAsset* targets[2]       = {&output->get(0), &output->get(1)};
    ds::ColorAttachmentInstance* attachments[2] = {
        &output1Descriptor.value(),
        &output2Descriptor.value(),
    };
    unsigned int currentTarget = 1;

    scene::SceneRenderContext renderCtx(ctx.commandBuffer,
                                        ctx.observerIndex,
                                        targets[0]->getViewport(),
                                        RenderPhase::PostProcess,
                                        targets[0]->getRenderPassId(),
                                        ctx.renderingToRenderTexture);

    VkPipelineLayout layout = filterHighlightPipeline->pipelineLayout().rawLayout();
    UniformPayload pushConstants(*renderer);

    // copy only highlights to first target
    targets[0]->beginRender(ctx.commandBuffer, true);
    filterHighlightPipeline->bind(ctx.commandBuffer, Config::RenderPassIds::BloomPass, 0);
    inputAttachmentDescriptor->bind(ctx.commandBuffer, layout, 0);
    vkCmdPushConstants(ctx.commandBuffer,
                       layout,
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,
                       sizeof(UniformPayload),
                       &pushConstants);
    indexBuffer.bindAndDraw(ctx.commandBuffer);
    targets[0]->finishRender(ctx.commandBuffer);

    // apply blur passes
    blurPipeline->bind(ctx.commandBuffer, Config::RenderPassIds::BloomPass, 0);
    for (unsigned int i = 0; i < renderer->getSettings().getBloomPassCount() * 2; ++i) {
        pushConstants.horizontal = i % 2;
        vkCmdPushConstants(ctx.commandBuffer,
                           layout,
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(UniformPayload),
                           &pushConstants);

        const std::uint32_t nextTarget = (currentTarget + 1) % 2;
        targets[currentTarget]->beginRender(ctx.commandBuffer, false);
        attachments[nextTarget]->bind(ctx.commandBuffer, layout, 0);
        indexBuffer.bindAndDraw(ctx.commandBuffer);
        targets[currentTarget]->finishRender(ctx.commandBuffer);
        currentTarget = nextTarget;
    }
}

void BloomTask::update(float) {}

} // namespace rgi
} // namespace rc
} // namespace bl
