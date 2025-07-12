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
                                const VkImageUsageFlags* usages, VkSampleCountFlagBits samples) {
    owner = &vulkanState;
    attachments.setRenderExtent(size);

    std::array<VkImageAspectFlags, MaxBufferCount> aspects;
    for (std::uint32_t i = 0; i < count; ++i) {
        aspects[i] = VulkanState::guessImageAspect(bufferFormats[i], usages[i]);
        buffers[i].create(vulkanState,
                          Image::Type::Image2D,
                          bufferFormats[i],
                          usages[i],
                          size,
                          aspects[i],
                          VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                          0,
                          VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM,
                          samples);
        images[i] = buffers[i].getImage();
        views[i]  = buffers[i].getView();
    }

    attachments.init(images, views, aspects, buffers[0].getLayerCount(), count);
    attachments.setOutputIndex(oi);
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