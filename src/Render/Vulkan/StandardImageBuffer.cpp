#include <BLIB/Render/Vulkan/StandardImageBuffer.hpp>

namespace bl
{
namespace render
{
StandardImageBuffer::StandardImageBuffer()
: owner(nullptr) {}

StandardImageBuffer::~StandardImageBuffer() { destroy(); }

void StandardImageBuffer::create(VulkanState& vs, const VkExtent2D& size) {
    destroy();
    owner              = &vs;
    attachments.extent = size;

    VkImage& colorImage         = attachments.imageHandles[StandardAttachmentSet::ColorIndex];
    VkImageView& colorImageView = attachments.imageViewHandles[StandardAttachmentSet::ColorIndex];
    VkImage& depthImage         = attachments.imageHandles[StandardAttachmentSet::DepthIndex];
    VkImageView& depthImageView = attachments.imageViewHandles[StandardAttachmentSet::DepthIndex];

    // create color image handle
    VkImageCreateInfo colorImageCreate{};
    colorImageCreate.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    colorImageCreate.imageType     = VK_IMAGE_TYPE_2D;
    colorImageCreate.extent.width  = size.width;
    colorImageCreate.extent.height = size.height;
    colorImageCreate.extent.depth  = 1;
    colorImageCreate.mipLevels     = 1;
    colorImageCreate.arrayLayers   = 1;
    colorImageCreate.format        = VK_FORMAT_R8G8B8A8_SRGB;
    colorImageCreate.tiling        = VK_IMAGE_TILING_OPTIMAL;
    colorImageCreate.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorImageCreate.usage       = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    colorImageCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    colorImageCreate.samples     = VK_SAMPLE_COUNT_1_BIT;
    colorImageCreate.flags       = 0; // Optional
    if (vkCreateImage(vs.device, &colorImageCreate, nullptr, &colorImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    currentColorLayout = colorImageCreate.initialLayout;

    // create depth image handle
    VkImageCreateInfo depthImageCreate{};
    depthImageCreate.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depthImageCreate.imageType     = VK_IMAGE_TYPE_2D;
    depthImageCreate.extent.width  = size.width;
    depthImageCreate.extent.height = size.height;
    depthImageCreate.extent.depth  = 1;
    depthImageCreate.mipLevels     = 1;
    depthImageCreate.arrayLayers   = 1;
    depthImageCreate.format        = findDepthFormat(vs);
    depthImageCreate.tiling        = VK_IMAGE_TILING_OPTIMAL;
    depthImageCreate.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImageCreate.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageCreate.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    depthImageCreate.samples       = VK_SAMPLE_COUNT_1_BIT;
    depthImageCreate.flags         = 0; // Optional
    if (vkCreateImage(vs.device, &depthImageCreate, nullptr, &depthImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    currentDepthLayout = depthImageCreate.initialLayout;

    // allocate memory
    VkMemoryRequirements colorImageMemReqs;
    vkGetImageMemoryRequirements(vs.device, colorImage, &colorImageMemReqs);
    VkMemoryRequirements depthImageMemReqs;
    vkGetImageMemoryRequirements(vs.device, depthImage, &depthImageMemReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = colorImageMemReqs.size + depthImageMemReqs.size;
    allocInfo.memoryTypeIndex =
        vs.findMemoryType(colorImageMemReqs.memoryTypeBits | depthImageMemReqs.memoryTypeBits,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(vs.device, &allocInfo, nullptr, &gpuMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(vs.device, colorImage, gpuMemory, 0);
    vkBindImageMemory(vs.device, depthImage, gpuMemory, colorImageMemReqs.size);

    // create image views
    colorImageView = vs.createImageView(colorImage, colorImageCreate.format);
    depthImageView =
        vs.createImageView(depthImage, depthImageCreate.format, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void StandardImageBuffer::destroy() {
    if (owner != nullptr) {
        vkDestroyImageView(owner->device, attachments.colorImageView(), nullptr);
        vkDestroyImage(owner->device, attachments.colorImage(), nullptr);
        vkDestroyImageView(owner->device, attachments.depthImageView(), nullptr);
        vkDestroyImage(owner->device, attachments.depthImage(), nullptr);
        vkFreeMemory(owner->device, gpuMemory, nullptr);
        owner = nullptr;
    }
}

void StandardImageBuffer::prepareForSampling(VkCommandBuffer cb) {
    if (currentColorLayout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        VkImageMemoryBarrier barrier{};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = currentColorLayout;
        barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = attachments.colorImage();
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.srcAccessMask                   = 0;
        barrier.dstAccessMask                   = 0;
        vkCmdPipelineBarrier(cb,
                             VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, // wait for rendering to complete
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // block rendering
                             0,                                  // no dependency flags
                             0,                                  // 0 memory barriers
                             nullptr,
                             0, // 0 buffer memory barriers
                             nullptr,
                             1,
                             &barrier);

        currentColorLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
}

VkFormat StandardImageBuffer::findDepthFormat(VulkanState& vs) {
    return vs.findSupportedFormat(
        {VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace render
} // namespace bl
