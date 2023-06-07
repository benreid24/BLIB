#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>

#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
namespace vk
{
AttachmentBuffer::AttachmentBuffer()
: vulkanState(nullptr)
, size{0, 0}
, alloc(nullptr)
, imageHandle(nullptr)
, viewHandle(nullptr) {}

AttachmentBuffer::~AttachmentBuffer() {
    if (vulkanState) { destroy(); }
}

void AttachmentBuffer::create(VulkanState& vs, VkFormat format, VkImageUsageFlags usage,
                              const VkExtent2D& extent) {
    if (size.width == extent.width && size.height == extent.height) { return; }
    size = extent;

    if (vulkanState) { destroy(); }
    vulkanState = &vs;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

    VkImageCreateInfo depthImageCreate{};
    depthImageCreate.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depthImageCreate.imageType     = VK_IMAGE_TYPE_2D;
    depthImageCreate.extent.width  = size.width;
    depthImageCreate.extent.height = size.height;
    depthImageCreate.extent.depth  = 1;
    depthImageCreate.mipLevels     = 1;
    depthImageCreate.arrayLayers   = 1;
    depthImageCreate.format        = format;
    depthImageCreate.tiling        = VK_IMAGE_TILING_OPTIMAL;
    depthImageCreate.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImageCreate.usage         = usage;
    depthImageCreate.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    depthImageCreate.samples       = VK_SAMPLE_COUNT_1_BIT;
    depthImageCreate.flags         = 0; // Optional
    if (vmaCreateImage(
            vs.vmaAllocator, &depthImageCreate, &allocInfo, &imageHandle, &alloc, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create depth buffer");
    }

    const bool depth = (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
    viewHandle       = vs.createImageView(imageHandle,
                                    depthImageCreate.format,
                                    depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
}

void AttachmentBuffer::destroy() {
    vkDestroyImageView(vulkanState->device, viewHandle, nullptr);
    vmaDestroyImage(vulkanState->vmaAllocator, imageHandle, alloc);
    vulkanState = nullptr;
}

} // namespace vk
} // namespace render
} // namespace bl
