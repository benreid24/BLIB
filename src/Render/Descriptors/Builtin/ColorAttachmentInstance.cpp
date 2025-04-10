#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentInstance.hpp>

#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
ColorAttachmentInstance::ColorAttachmentInstance(vk::VulkanState& vulkanState,
                                                 VkDescriptorSetLayout layout)
: DescriptorSetInstance(DescriptorSetInstance::Bindless, DescriptorSetInstance::SpeedAgnostic)
, vulkanState(vulkanState)
, layout(layout) {
    descriptorSets.emptyInit(vulkanState);
    dsAlloc = vulkanState.descriptorPool.allocate(
        layout, descriptorSets.rawData(), descriptorSets.size());
    cachedViews.emptyInit(vulkanState);
}

ColorAttachmentInstance::~ColorAttachmentInstance() {
    vulkanState.descriptorPool.release(dsAlloc, descriptorSets.rawData());
}

void ColorAttachmentInstance::bindForPipeline(scene::SceneRenderContext& ctx,
                                              VkPipelineLayout layout, std::uint32_t setIndex,
                                              UpdateSpeed) const {
    const vk::Framebuffer& fb = framebuffers[vulkanState.currentFrameIndex()];
    VkImageView& cachedView   = cachedViews.getRaw(vulkanState.currentFrameIndex());

    // update descriptor on change
    if (cachedView != fb.getAttachmentSet().imageViews()[attachmentIndex]) {
        cachedView = fb.getAttachmentSet().imageViews()[attachmentIndex];

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = cachedView;
        imageInfo.sampler     = sampler;

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = descriptorSets.current();
        write.dstBinding      = 0;
        write.dstArrayElement = 0;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo      = &imageInfo;

        vkUpdateDescriptorSets(vulkanState.device, 1, &write, 0, nullptr);
    }

    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(),
                            0,
                            nullptr);
}

void ColorAttachmentInstance::initAttachments(const vk::Framebuffer* fbs, std::uint32_t ai,
                                              VkSampler smp) {
    attachmentIndex = ai;
    framebuffers    = fbs;
    sampler         = smp;

    std::array<VkDescriptorImageInfo, Config::MaxConcurrentFrames> imageInfos{};
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView = framebuffers[i].getAttachmentSet().imageViews()[attachmentIndex];
        imageInfos[i].sampler   = sampler;
        cachedViews.getRaw(i)   = imageInfos[i].imageView;
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

    vkUpdateDescriptorSets(
        vulkanState.device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void ColorAttachmentInstance::updateAttachment(const vk::Framebuffer& framebuffer,
                                               std::uint32_t attachmentIndex, VkSampler sampler) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = framebuffer.getAttachmentSet().imageViews()[attachmentIndex];
    imageInfo.sampler     = sampler;

    VkWriteDescriptorSet write{};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet          = descriptorSets.current();
    write.dstBinding      = 0;
    write.dstArrayElement = 0;
    write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo      = &imageInfo;

    vkUpdateDescriptorSets(vulkanState.device, 1, &write, 0, nullptr);
}

} // namespace ds
} // namespace rc
} // namespace bl
