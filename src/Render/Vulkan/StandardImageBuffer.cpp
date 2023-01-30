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

    // create image handle
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

    // TODO - create depth handle

    // allocate memory
    VkMemoryRequirements colorImageMemReqs;
    vkGetImageMemoryRequirements(vs.device, colorImage, &colorImageMemReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = colorImageMemReqs.size; // TODO - add depth image add
    allocInfo.memoryTypeIndex =
        vs.findMemoryType(colorImageMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(vs.device, &allocInfo, nullptr, &gpuMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(vs.device, colorImage, gpuMemory, 0);
    // TODO - bind depth image with offset

    // create image views
    colorImageView = vs.createImageView(colorImage, colorImageCreate.format);
    // TODO - depth buffer view

    // TODO - even have sampler here?
    // create sampler
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = VK_FILTER_NEAREST;
    samplerInfo.minFilter               = VK_FILTER_NEAREST;
    samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.anisotropyEnable        = VK_TRUE;
    samplerInfo.maxAnisotropy           = vs.physicalDeviceProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE; // TODO - correct?
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias              = 0.0f;
    samplerInfo.minLod                  = 0.0f;
    samplerInfo.maxLod                  = 0.0f;
    if (vkCreateSampler(vs.device, &samplerInfo, nullptr, &colorSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void StandardImageBuffer::destroy() {
    if (owner != nullptr) {
        vkDestroySampler(owner->device, colorSampler, nullptr);
        vkDestroyImageView(owner->device, attachments.colorImageView(), nullptr);
        vkDestroyImage(owner->device, attachments.colorImage(), nullptr);
        vkFreeMemory(owner->device, gpuMemory, nullptr);
        owner = nullptr;
    }
}

void StandardImageBuffer::prepareForRendering(VkCommandBuffer cb) {
    // TODO - consider doing this in the render pass instead?
    if (currentColorLayout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        VkImageMemoryBarrier barrier{};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = currentColorLayout;
        barrier.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
                             0,                                 // dont wait on anything
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, // block rendering
                             0,                                 // no dependency flags
                             0,                                 // 0 memory barriers
                             nullptr,
                             0, // 0 buffer memory barriers
                             nullptr,
                             1,
                             &barrier);

        currentColorLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
}

void StandardImageBuffer::prepareForSampling(VkCommandBuffer cb) {
    // TODO - consider doing this in the render pass instead?
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

} // namespace render
} // namespace bl
