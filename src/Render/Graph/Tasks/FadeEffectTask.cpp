#include <BLIB/Render/Graph/Tasks/FadeEffectTask.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Builtin/FadeEffectFactory.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/StandardTargetAsset.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FadeEffectTask::FadeEffectTask(float fadeTime, float start, float end)
: renderer(nullptr)
, cachedView(nullptr) {
    assetTags.concreteOutputs.emplace_back(rg::AssetTags::FinalFrameOutput);
    assetTags.createdOutput = rg::AssetTags::PostFXOutput;
    assetTags.requiredInputs.emplace_back(rg::AssetTags::RenderedSceneOutput);

    fade(fadeTime, start, end);
}

FadeEffectTask::~FadeEffectTask() {
    if (renderer) {
        renderer->vulkanState().cleanupManager.add(
            [r      = renderer,
             alloc  = dsAlloc,
             dsCopy = vk::PerFrame<VkDescriptorSet>::copy(descriptorSets)]() {
                r->vulkanState().descriptorPool.release(alloc, dsCopy.rawData());
            });
    }
}

void FadeEffectTask::fadeTo(float fadeTime, float factorEnd) {
    fadeEnd   = factorEnd;
    fadeSpeed = (fadeEnd - factor) / fadeTime;
}

void FadeEffectTask::fade(float fadeTime, float fadeStart, float factorEnd) {
    factor    = fadeStart;
    fadeEnd   = factorEnd;
    fadeSpeed = (fadeEnd - factor) / fadeTime;
}

void FadeEffectTask::create(engine::Engine&, Renderer& r) {
    renderer = &r;

    // fetch pipeline
    pipeline = &r.pipelineCache().getPipeline(Config::PipelineIds::FadeEffect);

    // alloc descriptor sets
    const VkDescriptorSetLayout setLayout =
        r.descriptorFactoryCache().getFactory<ds::FadeEffectFactory>()->getDescriptorLayout();
    descriptorSets.emptyInit(r.vulkanState());
    dsAlloc = r.vulkanState().descriptorPool.allocate(
        setLayout, descriptorSets.rawData(), Config::MaxConcurrentFrames);

    // create index buffer
    indexBuffer.create(r.vulkanState(), 4, 6);
    indexBuffer.indices()  = {0, 1, 3, 1, 2, 3};
    indexBuffer.vertices() = {prim::Vertex({-1.f, -1.f, 1.0f}, {0.f, 0.f}),
                              prim::Vertex({1.f, -1.f, 1.0f}, {1.f, 0.f}),
                              prim::Vertex({1.f, 1.f, 1.0f}, {1.f, 1.f}),
                              prim::Vertex({-1.f, 1.f, 1.0f}, {0.f, 1.f})};
    indexBuffer.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);
}

void FadeEffectTask::onGraphInit() {
    StandardTargetAsset* src =
        dynamic_cast<StandardTargetAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!src) { throw std::runtime_error("Got bad input"); }

    output = dynamic_cast<FramebufferAsset*>(&assets.output->asset.get());
    if (!output) { throw std::runtime_error("Got bad output"); }

    // update descriptor sets
    std::array<VkDescriptorImageInfo, Config::MaxConcurrentFrames> imageInfos{};
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView   = src->getImages().getRaw(i).attachmentSet().colorImageView();
        imageInfos[i].sampler     = renderer->vulkanState().samplerCache.filteredEdgeClamped();
    }

    std::array<VkWriteDescriptorSet, Config::MaxConcurrentFrames> descriptorWrites{};
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
        descriptorWrites[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet          = descriptorSets.getRaw(i);
        descriptorWrites[i].dstBinding      = 0;
        descriptorWrites[i].dstArrayElement = 0;
        descriptorWrites[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[i].descriptorCount = 1;
        descriptorWrites[i].pImageInfo      = &imageInfos[i];
    }

    vkUpdateDescriptorSets(renderer->vulkanState().device,
                           descriptorWrites.size(),
                           descriptorWrites.data(),
                           0,
                           nullptr);
}

void FadeEffectTask::execute(const rg::ExecutionContext& ctx) {
    if (!ctx.isFinalStep) {
        output->currentFramebuffer().beginRender(ctx.commandBuffer,
                                                 output->scissor,
                                                 output->clearColors,
                                                 output->clearColorCount,
                                                 false,
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

    VkBuffer vb            = indexBuffer.vertexBufferHandle();
    VkDeviceSize offsets[] = {0};
    pipeline->bind(ctx.commandBuffer, output->renderPassId);
    vkCmdBindVertexBuffers(ctx.commandBuffer, 0, 1, &vb, offsets);
    vkCmdBindIndexBuffer(
        ctx.commandBuffer, indexBuffer.indexBufferHandle(), 0, buf::IndexBuffer::IndexType);
    vkCmdBindDescriptorSets(ctx.commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->pipelineLayout().rawLayout(),
                            0,
                            1,
                            &descriptorSets.current(),
                            0,
                            nullptr);
    vkCmdPushConstants(ctx.commandBuffer,
                       pipeline->pipelineLayout().rawLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,
                       sizeof(float),
                       &factor);
    vkCmdDrawIndexed(ctx.commandBuffer,
                     indexBuffer.getDrawParameters().indexCount,
                     1,
                     0,
                     indexBuffer.getDrawParameters().vertexOffset,
                     indexBuffer.getDrawParameters().firstInstance);

    if (!ctx.isFinalStep) { output->currentFramebuffer().finishRender(ctx.commandBuffer); }
}

void FadeEffectTask::update(float dt) {
    if (factor < fadeEnd) {
        factor += fadeSpeed * dt;
        if (factor >= fadeEnd) {
            fadeSpeed = 0.f;
            factor    = fadeEnd;
        }
    }
    else if (factor > fadeEnd) {
        factor += fadeSpeed * dt;
        if (factor <= fadeEnd) {
            fadeSpeed = 0.f;
            factor    = fadeEnd;
        }
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl
