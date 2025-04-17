#include <BLIB/Render/Vulkan/Image.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
namespace
{
VkImageCreateFlags getCreateFlags(Image::Type type) {
    switch (type) {
    case Image::Type::Cubemap:
        return VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    case Image::Type::Image2D:
    default:
        return 0;
    }
}
} // namespace

Image::Image()
: vulkanState(nullptr)
, size{0, 0}
, alloc(nullptr)
, imageHandle(nullptr)
, viewHandle(nullptr) {}

Image::~Image() { deferDestroy(); }

void Image::create(VulkanState& vs, Type t, VkFormat fmt, VkImageUsageFlags usg,
                   const VkExtent2D& extent, VkImageAspectFlags asp, VmaAllocationCreateFlags af,
                   VkMemoryPropertyFlags mem, VkImageCreateFlags ef) {
    if (vulkanState && size.width == extent.width && size.height == extent.height) {
        //
        return;
    }
    size = extent;

    deferDestroy();
    vulkanState      = &vs;
    type             = t;
    usage            = usg;
    aspect           = asp;
    format           = fmt;
    allocFlags       = af;
    extraCreateFlags = ef;
    memoryLocation   = mem;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = memoryLocation;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = allocFlags;

    VkImageCreateInfo createInfo{};
    createInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType     = VK_IMAGE_TYPE_2D;
    createInfo.extent.width  = size.width;
    createInfo.extent.height = size.height;
    createInfo.extent.depth  = 1;
    createInfo.mipLevels     = 1;
    createInfo.arrayLayers   = getLayerCount();
    createInfo.format        = format;
    createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage         = usage;
    createInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags         = getCreateFlags(type) | extraCreateFlags;
    if (vmaCreateImage(vs.vmaAllocator, &createInfo, &allocInfo, &imageHandle, &alloc, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image");
    }

    viewHandle    = vs.createImageView(imageHandle, format, aspect, getLayerCount(), getViewType());
    currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}

void Image::resize(const glm::u32vec2& newSize, bool copyContents) {
    // cache data needed to delete original image
    VkImage oldImage = imageHandle;

    // queue deletion of original resources
    vulkanState->cleanupManager.add(
        [vulkanState = vulkanState, oldImage, oldAlloc = alloc, oldView = viewHandle]() {
            vkDestroyImageView(vulkanState->device, oldView, nullptr);
            vmaDestroyImage(vulkanState->vmaAllocator, oldImage, oldAlloc);
        });

    // transition original image to transfer source layout
    vulkanState->transitionImageLayout(
        imageHandle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    currentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    // create new image
    const glm::u32vec2 oldSize(size.width, size.height);
    create(*vulkanState,
           type,
           format,
           usage,
           {newSize.x, newSize.y},
           aspect,
           allocFlags,
           extraCreateFlags);

    // copy from old to new
    auto cb = vulkanState->sharedCommandPool.createBuffer();

    VkImageCopy copyInfo{};
    copyInfo.extent.width                  = std::min(oldSize.x, newSize.x);
    copyInfo.extent.height                 = std::min(oldSize.y, oldSize.y);
    copyInfo.extent.depth                  = 1;
    copyInfo.srcSubresource.aspectMask     = aspect;
    copyInfo.srcSubresource.mipLevel       = 0;
    copyInfo.srcSubresource.baseArrayLayer = 0;
    copyInfo.srcSubresource.layerCount     = getLayerCount();
    copyInfo.dstSubresource                = copyInfo.srcSubresource;

    vkCmdCopyImage(cb,
                   oldImage,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   imageHandle,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &copyInfo);

    cb.submit();
}

void Image::destroy() {
    vkDestroyImageView(vulkanState->device, viewHandle, nullptr);
    vmaDestroyImage(vulkanState->vmaAllocator, imageHandle, alloc);
    vulkanState = nullptr;
}

void Image::deferDestroy() {
    if (vulkanState) {
        vulkanState->cleanupManager.add(
            [vs = vulkanState, vh = viewHandle, img = imageHandle, alloc = alloc]() {
                vkDestroyImageView(vs->device, vh, nullptr);
                vmaDestroyImage(vs->vmaAllocator, img, alloc);
            });
        vulkanState = nullptr;
    }
}

void Image::clearAndPrepareForSampling(VkClearColorValue color) {
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

VkImageViewType Image::getViewType() const {
    switch (type) {
    case Type::Cubemap:
        return VK_IMAGE_VIEW_TYPE_CUBE;
    case Type::Image2D:
    default:
        return VK_IMAGE_VIEW_TYPE_2D;
    }
}

std::uint32_t Image::getLayerCount() const {
    switch (type) {
    case Type::Cubemap:
        return 6;
    case Type::Image2D:
    default:
        return 1;
    }
}

void Image::transitionLayout(VkImageLayout newLayout, bool undefinedLayout) {
    vulkanState->transitionImageLayout(imageHandle,
                                       !undefinedLayout ? currentLayout : VK_IMAGE_LAYOUT_UNDEFINED,
                                       newLayout,
                                       getLayerCount());
    currentLayout = newLayout;
}

void Image::transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout,
                             bool undefinedLayout) {
    vulkanState->transitionImageLayout(commandBuffer,
                                       imageHandle,
                                       !undefinedLayout ? currentLayout : VK_IMAGE_LAYOUT_UNDEFINED,
                                       newLayout,
                                       getLayerCount());
    currentLayout = newLayout;
}

void Image::notifyNewLayout(VkImageLayout nl) { currentLayout = nl; }

} // namespace vk
} // namespace rc
} // namespace bl
