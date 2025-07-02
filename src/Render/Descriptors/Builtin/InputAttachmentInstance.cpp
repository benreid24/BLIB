#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
InputAttachmentInstance::InputAttachmentInstance(vk::VulkanState& vulkanState,
                                                 VkDescriptorSetLayout layout)
: DescriptorSetInstance(DescriptorSetInstance::Bindless, DescriptorSetInstance::SpeedAgnostic)
, vulkanState(vulkanState)
, layout(layout) {
    descriptorSets.emptyInit(vulkanState);
    dsAlloc = vulkanState.descriptorPool.allocate(
        layout, descriptorSets.rawData(), descriptorSets.size());
    cachedViews.emptyInit(vulkanState);
}

InputAttachmentInstance::~InputAttachmentInstance() {
    vulkanState.descriptorPool.release(dsAlloc, descriptorSets.rawData());
}

void InputAttachmentInstance::bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                   std::uint32_t setIndex) const {
    const vk::AttachmentSet& attachments = *source.get(vulkanState.currentFrameIndex());
    auto& cached                         = cachedViews.getRaw(vulkanState.currentFrameIndex());

    bool changed = false;
    for (unsigned int i = 0; i < attachments.size(); ++i) {
        if (cached[i] != attachments.imageViews()[i]) {
            cached[i] = attachments.imageViews()[i];
            changed   = true;
            break;
        }
    }

    // update descriptor on change
    if (changed) {
        std::array<VkDescriptorImageInfo, 8> imageInfo{};
        for (unsigned int i = 0; i < attachments.size(); ++i) {
            imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo[i].imageView   = attachments.imageViews()[i];
            imageInfo[i].sampler     = sampler;
        }

        std::array<VkWriteDescriptorSet, 8> write{};
        for (unsigned int i = 0; i < attachments.size(); ++i) {
            write[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write[i].dstSet          = descriptorSets.getRaw(vulkanState.currentFrameIndex());
            write[i].dstBinding      = i;
            write[i].dstArrayElement = 0;
            write[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write[i].descriptorCount = 1;
            write[i].pImageInfo      = &imageInfo[i];
        }

        vkUpdateDescriptorSets(vulkanState.device, attachments.size(), write.data(), 0, nullptr);
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

void InputAttachmentInstance::bindForPipeline(scene::SceneRenderContext& ctx,
                                              VkPipelineLayout layout, std::uint32_t setIndex,
                                              UpdateSpeed) const {
    bind(ctx.getCommandBuffer(), layout, setIndex);
}

void InputAttachmentInstance::initAttachments(const vk::AttachmentSet* set, VkSampler smp) {
    source.init(set);
    sampler = smp;
    commonInit();
}

void InputAttachmentInstance::initAttachments(
    const std::array<const vk::AttachmentSet*, cfg::Limits::MaxConcurrentFrames>& sets,
    VkSampler smp) {
    source.init(sets);
    sampler = smp;
    commonInit();
}

void InputAttachmentInstance::commonInit() {
    std::array<VkDescriptorImageInfo, cfg::Limits::MaxConcurrentFrames * 8> imageInfos{};
    for (unsigned int i = 0; i < cfg::Limits::MaxConcurrentFrames; ++i) {
        for (unsigned int j = 0; j < source.get(i)->size(); ++j) {
            const unsigned int k      = j * 2 + i;
            imageInfos[k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[k].imageView   = source.get(i)->imageViews()[j];
            imageInfos[k].sampler     = sampler;
            cachedViews.getRaw(i)[j]  = imageInfos[i].imageView;
        }
    }

    std::array<VkWriteDescriptorSet, cfg::Limits::MaxConcurrentFrames * 8> descriptorWrites{};
    for (unsigned int i = 0; i < cfg::Limits::MaxConcurrentFrames; ++i) {
        for (unsigned int j = 0; j < source.get(i)->size(); ++j) {
            const unsigned int k                = j * 2 + i;
            descriptorWrites[k].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[k].dstSet          = descriptorSets.getRaw(i);
            descriptorWrites[k].dstBinding      = j;
            descriptorWrites[k].dstArrayElement = 0;
            descriptorWrites[k].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[k].descriptorCount = 1;
            descriptorWrites[k].pImageInfo      = &imageInfos[i];
        }
    }

    vkUpdateDescriptorSets(vulkanState.device,
                           cfg::Limits::MaxConcurrentFrames * source.get(0)->size(),
                           descriptorWrites.data(),
                           0,
                           nullptr);
}

} // namespace ds
} // namespace rc
} // namespace bl
