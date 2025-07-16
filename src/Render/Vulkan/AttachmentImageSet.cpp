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
                                const VkExtent2D& size, const VkFormat* bufferFormats,
                                const VkImageUsageFlags* usages, VkSampleCountFlagBits samples,
                                std::uint32_t firstResolveAttachment) {
    owner = &vulkanState;
    attachments.setRenderExtent(size);

    std::array<VkImageAspectFlags, MaxBufferCount> aspects;
    for (std::uint32_t i = 0; i < count; ++i) {
        aspects[i] = VulkanState::guessImageAspect(bufferFormats[i], usages[i]);
        buffers[i].create(vulkanState,
                          {.type       = ImageOptions::Type::Image2D,
                           .format     = bufferFormats[i],
                           .usage      = usages[i],
                           .extent     = size,
                           .aspect     = aspects[i],
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

} // namespace vk
} // namespace rc
} // namespace bl