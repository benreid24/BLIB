#include <BLIB/Render/Vulkan/AttachmentImageSet.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
AttachmentImageSet::AttachmentImageSet()
: owner(nullptr) {}

void AttachmentImageSet::create(VulkanState& vulkanState, unsigned int count,
                                const VkExtent2D& size,
                                const vk::SemanticTextureFormat* bufferFormats,
                                const VkImageUsageFlags* usages, VkSampleCountFlagBits samples,
                                std::uint32_t firstResolveAttachment) {
    owner = &vulkanState;
    attachments.setRenderExtent(size);

    std::array<VkImageAspectFlags, MaxBufferCount> aspects;
    for (std::uint32_t i = 0; i < count; ++i) {
        formats[i] = bufferFormats[i];
        aspects[i] = VulkanState::guessImageAspect(bufferFormats[i]);
        buffers[i].create(vulkanState,
                          {.type   = ImageOptions::Type::Image2D,
                           .format = vulkanState.textureFormatManager.getFormat(bufferFormats[i]),
                           .usage  = usages[i],
                           .extent = size,
                           .aspect = aspects[i],
                           .allocFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                           .samples    = samples});
        images[i] = buffers[i].getImage();
        views[i]  = buffers[i].getView();
    }

    attachments.init(
        count, images.data(), views.data(), aspects.data(), size, buffers[0].getLayerCount());
    attachments.setOutputIndex(firstResolveAttachment);
}

void AttachmentImageSet::destroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.destroy(); }
    }
}

void AttachmentImageSet::deferDestroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.deferDestroy(); }
    }
}

bool AttachmentImageSet::recreateForFormatChange() {
    if (owner == nullptr) { return false; }

    bool recreated = false;
    std::array<VkImageAspectFlags, MaxBufferCount> aspects;
    for (std::uint32_t i = 0; i < attachments.getAttachmentCount(); ++i) {
        const VkFormat actualFormat = owner->textureFormatManager.getFormat(formats[i]);
        if (buffers[i].getFormat() != actualFormat) {
            recreated  = true;
            aspects[i] = VulkanState::guessImageAspect(formats[i]);
            buffers[i].create(*owner,
                              {.type       = ImageOptions::Type::Image2D,
                               .format     = actualFormat,
                               .usage      = buffers[i].getUsage(),
                               .extent     = attachments.getRenderExtent(),
                               .aspect     = aspects[i],
                               .allocFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                               .samples    = buffers[i].getSampleCount()});
            images[i] = buffers[i].getImage();
            views[i]  = buffers[i].getView();
        }
    }
    if (recreated) {
        attachments.init(attachments.getAttachmentCount(),
                         images.data(),
                         views.data(),
                         aspects.data(),
                         attachments.getRenderExtent(),
                         buffers[0].getLayerCount());
    }
    return recreated;
}

} // namespace vk
} // namespace rc
} // namespace bl