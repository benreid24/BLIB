#include <BLIB/Render/Vulkan/Image.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <cmath>

namespace bl
{
namespace rc
{
namespace vk
{
namespace
{
VkImageCreateFlags getCreateFlags(ImageOptions::Type type) {
    switch (type) {
    case ImageOptions::Type::Cubemap:
        return VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    case ImageOptions::Type::Image2D:
    default:
        return 0;
    }
}
bool compareOptions(const ImageOptions& a, const ImageOptions& b) {
    return a.type == b.type && a.format == b.format && a.usage == b.usage &&
           a.extent.width == b.extent.width && a.extent.height == b.extent.height &&
           a.samples == b.samples && a.aspect == b.aspect && a.allocFlags == b.allocFlags &&
           a.memoryLocation == b.memoryLocation && a.extraCreateFlags == b.extraCreateFlags &&
           a.viewAspect == b.viewAspect;
}
} // namespace

std::uint32_t Image::computeMipLevelsForGeneration(std::uint32_t width, std::uint32_t height) {
    return static_cast<std::uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
}

std::uint32_t Image::determineMipLevelsFromExisting(std::uint32_t width, std::uint32_t height) {
    const std::uint32_t originalWidth = width * 2 / 3;
    return computeMipLevelsForGeneration(originalWidth, height);
}

Image::Image()
: vulkanState(nullptr)
, alloc(nullptr)
, imageHandle(nullptr)
, viewHandle(nullptr)
, currentLayout(VK_IMAGE_LAYOUT_UNDEFINED) {}

Image::~Image() { deferDestroy(); }

void Image::create(VulkanState& vs, const ImageOptions& options) {
    if (vulkanState && compareOptions(createOptions, options)) { return; }

    deferDestroy();
    vulkanState   = &vs;
    createOptions = options;
    if (createOptions.viewAspect == VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) {
        createOptions.viewAspect = createOptions.aspect;
    }

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = createOptions.memoryLocation;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = createOptions.allocFlags;

    VkImageCreateInfo createInfo{};
    createInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType     = VK_IMAGE_TYPE_2D;
    createInfo.extent.width  = createOptions.extent.width;
    createInfo.extent.height = createOptions.extent.height;
    createInfo.extent.depth  = 1;
    createInfo.mipLevels     = createOptions.mipLevels;
    createInfo.arrayLayers   = getLayerCount();
    createInfo.format        = createOptions.format;
    createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage         = createOptions.usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples       = createOptions.samples;
    createInfo.flags         = getCreateFlags(createOptions.type) | createOptions.extraCreateFlags;
    if (vmaCreateImage(vs.vmaAllocator, &createInfo, &allocInfo, &imageHandle, &alloc, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image");
    }

    viewHandle    = vs.createImageView(imageHandle,
                                    createOptions.format,
                                    createOptions.viewAspect,
                                    getLayerCount(),
                                    getViewType(),
                                    createOptions.mipLevels,
                                    0);
    currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}

void Image::resize(const glm::u32vec2& newSize, bool copyContents) {
    if (!vulkanState) {
        BL_LOG_WARN << "Cannot resize image before calling create()";
        return;
    }

    // cache data needed to delete original image
    VkImage oldImage = imageHandle;

    // transition original image to transfer source layout
    if (copyContents) {
        vulkanState->transitionImageLayout(imageHandle,
                                           VK_IMAGE_LAYOUT_UNDEFINED,
                                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                           getLayerCount(),
                                           createOptions.aspect);
        currentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }

    // create new image
    const glm::u32vec2 oldSize(createOptions.extent.width, createOptions.extent.height);
    create(*vulkanState, createOptions);
    currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // copy from old to new
    if (copyContents) {
        auto cb = vulkanState->sharedCommandPool.createBuffer();

        vulkanState->transitionImageLayout(cb,
                                           imageHandle,
                                           currentLayout,
                                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                           getLayerCount(),
                                           createOptions.aspect);
        currentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        if (createOptions.mipLevels > 1) {
            BL_LOG_WARN << "Copying mip image contents on resize is not "
                           "supported. Only the first level will be copied";
        }

        VkImageCopy copyInfo{};
        copyInfo.extent.width                  = std::min(oldSize.x, newSize.x);
        copyInfo.extent.height                 = std::min(oldSize.y, oldSize.y);
        copyInfo.extent.depth                  = 1;
        copyInfo.srcSubresource.aspectMask     = createOptions.aspect;
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
}

void Image::destroy() {
    if (vulkanState) {
        vkDestroyImageView(vulkanState->device, viewHandle, nullptr);
        vmaDestroyImage(vulkanState->vmaAllocator, imageHandle, alloc);
        vulkanState = nullptr;
    }
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

void Image::clearAndTransition(VkImageLayout finalLayout, VkClearValue color) {
    auto commandBuffer = vulkanState->sharedCommandPool.createBuffer();
    clearAndTransition(commandBuffer, finalLayout, color);
    commandBuffer.submit();
}

void Image::clearAndTransition(VkCommandBuffer commandBuffer, VkImageLayout finalLayout,
                               VkClearValue color) {
    vulkanState->transitionImageLayout(commandBuffer,
                                       imageHandle,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       getLayerCount(),
                                       createOptions.aspect,
                                       0,
                                       createOptions.mipLevels);

    VkImageSubresourceRange range{};
    range.aspectMask     = createOptions.aspect;
    range.baseArrayLayer = 0;
    range.baseMipLevel   = 0;
    range.layerCount     = getLayerCount();
    range.levelCount     = createOptions.mipLevels;

    if (createOptions.aspect & VK_IMAGE_ASPECT_DEPTH_BIT) {
        vkCmdClearDepthStencilImage(commandBuffer,
                                    imageHandle,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    &color.depthStencil,
                                    1,
                                    &range);
    }
    else {
        vkCmdClearColorImage(commandBuffer,
                             imageHandle,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             &color.color,
                             1,
                             &range);
    }
    vulkanState->transitionImageLayout(commandBuffer,
                                       imageHandle,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       finalLayout,
                                       getLayerCount(),
                                       createOptions.aspect,
                                       0,
                                       createOptions.mipLevels);
}

VkImageViewType Image::getViewType() const {
    switch (createOptions.type) {
    case ImageOptions::Type::Cubemap:
        return VK_IMAGE_VIEW_TYPE_CUBE;
    case ImageOptions::Type::Image2D:
    default:
        return VK_IMAGE_VIEW_TYPE_2D;
    }
}

std::uint32_t Image::getLayerCount() const {
    switch (createOptions.type) {
    case ImageOptions::Type::Cubemap:
        return 6;
    case ImageOptions::Type::Image2D:
    default:
        return 1;
    }
}

void Image::transitionLayout(VkImageLayout newLayout, bool undefinedLayout) {
    vulkanState->transitionImageLayout(imageHandle,
                                       !undefinedLayout ? currentLayout : VK_IMAGE_LAYOUT_UNDEFINED,
                                       newLayout,
                                       getLayerCount(),
                                       createOptions.aspect);
    currentLayout = newLayout;
}

void Image::transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout,
                             bool undefinedLayout) {
    vulkanState->transitionImageLayout(commandBuffer,
                                       imageHandle,
                                       !undefinedLayout ? currentLayout : VK_IMAGE_LAYOUT_UNDEFINED,
                                       newLayout,
                                       getLayerCount(),
                                       createOptions.aspect);
    currentLayout = newLayout;
}

void Image::notifyNewLayout(VkImageLayout nl) { currentLayout = nl; }

} // namespace vk
} // namespace rc
} // namespace bl
