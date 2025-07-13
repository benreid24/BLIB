#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentInstance.hpp>

#include <BLIB/Render/Config/Limits.hpp>
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

void ColorAttachmentInstance::bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                   std::uint32_t setIndex) const {
    const vk::AttachmentSet& attachments = *source.get(vulkanState.currentFrameIndex());
    VkImageView& cachedView              = cachedViews.getRaw(vulkanState.currentFrameIndex());

    // update descriptor on change
    if (cachedView != attachments.getImageView(attachmentIndex)) {
        cachedView = attachments.getImageView(attachmentIndex);

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

    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(),
                            0,
                            nullptr);
}

void ColorAttachmentInstance::bindForPipeline(scene::SceneRenderContext& ctx,
                                              VkPipelineLayout layout, std::uint32_t setIndex,
                                              UpdateSpeed) const {
    bind(ctx.getCommandBuffer(), layout, setIndex);
}

void ColorAttachmentInstance::initAttachments(const vk::AttachmentSet* set, std::uint32_t ai,
                                              VkSampler smp) {
    source.init(set);
    attachmentIndex = ai;
    sampler         = smp;
    commonInit();
}

void ColorAttachmentInstance::initAttachments(
    const std::array<const vk::AttachmentSet*, cfg::Limits::MaxConcurrentFrames>& sets,
    std::uint32_t ai, VkSampler smp) {
    source.init(sets);
    attachmentIndex = ai;
    sampler         = smp;
    commonInit();
}

void ColorAttachmentInstance::commonInit() {
    std::array<VkDescriptorImageInfo, cfg::Limits::MaxConcurrentFrames> imageInfos{};
    for (unsigned int i = 0; i < cfg::Limits::MaxConcurrentFrames; ++i) {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView   = source.get(i)->getImageView(attachmentIndex);
        imageInfos[i].sampler     = sampler;
        cachedViews.getRaw(i)     = imageInfos[i].imageView;
    }

    std::array<VkWriteDescriptorSet, cfg::Limits::MaxConcurrentFrames> descriptorWrites{};
    for (unsigned int i = 0; i < cfg::Limits::MaxConcurrentFrames; ++i) {
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

} // namespace ds
} // namespace rc
} // namespace bl
