#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>

#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
AttachmentBuffer::AttachmentBuffer()
: vulkanState(nullptr)
, size{0, 0}
, alloc(nullptr)
, imageHandle(nullptr)
, viewHandle(nullptr) {}

AttachmentBuffer::~AttachmentBuffer() { deferDestroy(); }

void AttachmentBuffer::create(VulkanState& vs, VkFormat format, VkImageUsageFlags usage,
                              const VkExtent2D& extent) {
    if (vulkanState && size.width == extent.width && size.height == extent.height) { return; }
    size = extent;

    deferDestroy();
    vulkanState = &vs;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

    VkImageCreateInfo createInfo{};
    createInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType     = VK_IMAGE_TYPE_2D;
    createInfo.extent.width  = size.width;
    createInfo.extent.height = size.height;
    createInfo.extent.depth  = 1;
    createInfo.mipLevels     = 1;
    createInfo.arrayLayers   = 1;
    createInfo.format        = format;
    createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage         = usage;
    createInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags         = 0; // Optional
    if (vmaCreateImage(vs.vmaAllocator, &createInfo, &allocInfo, &imageHandle, &alloc, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create depth buffer");
    }

    const bool depth = (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
    viewHandle       = vs.createImageView(imageHandle,
                                    createInfo.format,
                                    depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
}

void AttachmentBuffer::destroy() {
    vkDestroyImageView(vulkanState->device, viewHandle, nullptr);
    vmaDestroyImage(vulkanState->vmaAllocator, imageHandle, alloc);
    vulkanState = nullptr;
}

void AttachmentBuffer::deferDestroy() {
    if (vulkanState) {
        vulkanState->cleanupManager.add(
            [vs = vulkanState, vh = viewHandle, img = imageHandle, alloc = alloc]() {
                vkDestroyImageView(vs->device, vh, nullptr);
                vmaDestroyImage(vs->vmaAllocator, img, alloc);
            });
        vulkanState = nullptr;
    }
}

void AttachmentBuffer::clearAndPrepareForSampling(VkImageAspectFlags aspect,
                                                  VkClearColorValue color) {
    auto commandBuffer = vulkanState->sharedCommandPool.createBuffer();
    vulkanState->transitionImageLayout(commandBuffer,
                                       imageHandle,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageSubresourceRange range{};
    range.aspectMask     = aspect;
    range.baseArrayLayer = 0;
    range.baseMipLevel   = 0;
    range.layerCount     = 1;
    range.levelCount     = 1;

    vkCmdClearColorImage(
        commandBuffer, imageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);
    vulkanState->transitionImageLayout(commandBuffer,
                                       imageHandle,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    commandBuffer.submit();
}

} // namespace vk
} // namespace rc
} // namespace bl
