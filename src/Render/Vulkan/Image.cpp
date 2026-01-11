#include <BLIB/Render/Vulkan/Image.hpp>

#include <BLIB/Render/Renderer.hpp>
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
: renderer(nullptr)
, alloc(nullptr)
, imageHandle(nullptr)
, viewHandle(nullptr)
, currentLayout(VK_IMAGE_LAYOUT_UNDEFINED) {}

Image::~Image() { deferDestroy(); }

void Image::create(Renderer& r, const ImageOptions& options) {
    if (renderer && compareOptions(createOptions, options)) { return; }

    deferDestroy();
    renderer      = &r;
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
    if (vmaCreateImage(renderer->vulkanState().getVmaAllocator(),
                       &createInfo,
                       &allocInfo,
                       &imageHandle,
                       &alloc,
                       nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image");
    }

    viewHandle    = renderer->vulkanState().createImageView(imageHandle,
                                                         createOptions.format,
                                                         createOptions.viewAspect,
                                                         getLayerCount(),
                                                         getViewType(),
                                                         createOptions.mipLevels,
                                                         0);
    currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}

void Image::resize(const glm::u32vec2& newSize, bool copyContents) {
    if (!renderer) {
        BL_LOG_WARN << "Cannot resize image before calling create()";
        return;
    }

    // cache data needed to delete original image
    VkImage oldImage = imageHandle;

    // transition original image to transfer source layout
    if (copyContents) {
        auto cb = renderer->getSharedCommandPool().createBuffer();
        renderer->vulkanState().transitionImageLayout(cb,
                                                      imageHandle,
                                                      VK_IMAGE_LAYOUT_UNDEFINED,
                                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                      getLayerCount(),
                                                      createOptions.aspect);
        currentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        cb.submit();
    }

    // create new image
    const glm::u32vec2 oldSize(createOptions.extent.width, createOptions.extent.height);
    ImageOptions newOptions  = createOptions;
    newOptions.extent.width  = newSize.x;
    newOptions.extent.height = newSize.y;
    create(*renderer, newOptions);
    currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // copy from old to new
    if (copyContents && oldImage != imageHandle) {
        auto cb = renderer->getSharedCommandPool().createBuffer();

        renderer->vulkanState().transitionImageLayout(cb,
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
    if (renderer) {
        vkDestroyImageView(renderer->vulkanState().getDevice(), viewHandle, nullptr);
        vmaDestroyImage(renderer->vulkanState().getVmaAllocator(), imageHandle, alloc);
        renderer = nullptr;
    }
}

void Image::deferDestroy() {
    if (renderer) {
        renderer->getCleanupManager().add(
            [vs = &renderer->vulkanState(), vh = viewHandle, img = imageHandle, alloc = alloc]() {
                vkDestroyImageView(vs->getDevice(), vh, nullptr);
                vmaDestroyImage(vs->getVmaAllocator(), img, alloc);
            });
        renderer = nullptr;
    }
}

void Image::clearAndTransition(VkImageLayout finalLayout, VkClearValue color) {
    auto commandBuffer = renderer->getSharedCommandPool().createBuffer();
    clearAndTransition(commandBuffer, finalLayout, color);
    commandBuffer.submit();
}

void Image::clearAndTransition(VkCommandBuffer commandBuffer, VkImageLayout finalLayout,
                               VkClearValue color) {
    renderer->vulkanState().transitionImageLayout(commandBuffer,
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
    renderer->vulkanState().transitionImageLayout(commandBuffer,
                                                  imageHandle,
                                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                  finalLayout,
                                                  getLayerCount(),
                                                  createOptions.aspect,
                                                  0,
                                                  createOptions.mipLevels);
    currentLayout = finalLayout;
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
    auto cb = renderer->getSharedCommandPool().createBuffer();
    renderer->vulkanState().transitionImageLayout(cb,
                                                  imageHandle,
                                                  !undefinedLayout ? currentLayout :
                                                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                                  newLayout,
                                                  getLayerCount(),
                                                  createOptions.aspect);
    cb.submit();
    currentLayout = newLayout;
}

void Image::transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout,
                             bool undefinedLayout) {
    renderer->vulkanState().transitionImageLayout(commandBuffer,
                                                  imageHandle,
                                                  !undefinedLayout ? currentLayout :
                                                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                                  newLayout,
                                                  getLayerCount(),
                                                  createOptions.aspect);
    currentLayout = newLayout;
}

void Image::recordBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStages,
                          VkAccessFlags srcAccess, VkPipelineStageFlags dstStages,
                          VkAccessFlags dstAccess, VkImageLayout newLayout) {
    newLayout = (newLayout == VK_IMAGE_LAYOUT_UNDEFINED) ? currentLayout : newLayout;

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = currentLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = imageHandle;
    barrier.subresourceRange.aspectMask     = getAspect();
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = createOptions.mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = getLayerCount();
    barrier.srcAccessMask                   = srcAccess;
    barrier.dstAccessMask                   = dstAccess;
    vkCmdPipelineBarrier(
        commandBuffer, srcStages, dstStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    currentLayout = newLayout;
}

void Image::notifyNewLayout(VkImageLayout nl) { currentLayout = nl; }

bool Image::canGenMipMapsOnGpu() const {
    const VkFormatProperties props =
        renderer->vulkanState().getFormatProperties(createOptions.format);
    return (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) &&
           (props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) &&
           (props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);
}

void Image::genMipMapsOnGpu(VkImageLayout finalLayout) {
    auto commandBuffer = renderer->getSharedCommandPool().createBuffer();
    genMipMapsOnGpu(commandBuffer, finalLayout);
    commandBuffer.submit();
}

void Image::genMipMapsOnGpu(VkCommandBuffer commandBuffer, VkImageLayout finalLayout) {
    if (!canGenMipMapsOnGpu()) {
        BL_LOG_WARN << "Cannot generate mipmaps on GPU for image with format: "
                    << createOptions.format;
        return;
    }
    if (createOptions.type != ImageOptions::Type::Image2D) {
        BL_LOG_ERROR << "Cannot generate mipmaps for image type: " << createOptions.type;
        return;
    }

    // transition first level to transfer source
    renderer->vulkanState().transitionImageLayout(commandBuffer,
                                                  imageHandle,
                                                  currentLayout,
                                                  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                  1,
                                                  createOptions.aspect,
                                                  0,
                                                  1);

    // transfer remaining levels to transfer dest
    renderer->vulkanState().transitionImageLayout(commandBuffer,
                                                  imageHandle,
                                                  VK_IMAGE_LAYOUT_UNDEFINED,
                                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                  1,
                                                  createOptions.aspect,
                                                  1,
                                                  createOptions.mipLevels - 1);

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image                           = imageHandle;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    std::int32_t mipWidth  = createOptions.extent.width;
    std::int32_t mipHeight = createOptions.extent.height;

    for (std::uint32_t i = 1; i < createOptions.mipLevels; ++i) {
        // transition prior level to transfer source
        barrier.subresourceRange.baseMipLevel = i - 1;
        if (i > 1) { // we already transitioned the first level
            barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);
        }

        // copy prior level to current level and downsample
        VkImageBlit blit{};
        blit.srcOffsets[0]                 = {0, 0, 0};
        blit.srcOffsets[1]                 = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask     = createOptions.aspect;
        blit.srcSubresource.mipLevel       = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount     = 1;
        blit.dstOffsets[0]                 = {0, 0, 0};
        blit.dstOffsets[1]                 = {
            mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask     = createOptions.aspect;
        blit.dstSubresource.mipLevel       = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount     = 1;

        vkCmdBlitImage(commandBuffer,
                       imageHandle,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       imageHandle,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &blit,
                       VK_FILTER_LINEAR);

        // transition source level to final layout
        barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout     = finalLayout;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        if (mipWidth > 1) { mipWidth /= 2; }
        if (mipHeight > 1) { mipHeight /= 2; }
    }

    // transition last level to final layout
    barrier.subresourceRange.baseMipLevel = createOptions.mipLevels - 1;
    barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout                     = finalLayout;
    barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    currentLayout = finalLayout;
}

sf::Image Image::genMipMapsOnCpu(const sf::Image& image) {
    const std::uint32_t mipLevels =
        computeMipLevelsForGeneration(image.getSize().x, image.getSize().y);

    std::uint32_t sourceX   = 0;
    std::uint32_t sourceY   = 0;
    std::uint32_t mipWidth  = image.getSize().x / 2;
    std::uint32_t mipHeight = image.getSize().y / 2;
    std::uint32_t mipX      = image.getSize().x;
    std::uint32_t mipY      = 0;

    std::uint32_t totalMipHeight = 0;
    for (std::uint32_t i = 1; i < mipLevels; ++i) {
        totalMipHeight += mipHeight;
        if (mipHeight > 1) { mipHeight /= 2; }
    }
    mipHeight = image.getSize().y / 2;

    sf::Image result;
    result.create(image.getSize().x + mipWidth,
                  std::max(image.getSize().y, totalMipHeight),
                  sf::Color::Transparent);
    result.copy(image, 0, 0, sf::IntRect(0, 0, image.getSize().x, image.getSize().y));

    for (std::uint32_t i = 1; i < mipLevels; ++i) {
        for (std::uint32_t x = 0; x < mipWidth; ++x) {
            for (std::uint32_t y = 0; y < mipHeight; ++y) {
                std::uint32_t rSum = 0;
                std::uint32_t gSum = 0;
                std::uint32_t bSum = 0;
                std::uint32_t aSum = 0;
                std::uint32_t read = 0;

                const auto addColor = [&rSum, &gSum, &bSum, &aSum, &read, &result](
                                          std::uint32_t rx, std::uint32_t ry) {
                    if (rx < result.getSize().x && ry < result.getSize().y) {
                        ++read;
                        const sf::Color c = result.getPixel(rx, ry);
                        rSum += static_cast<std::uint32_t>(c.r);
                        gSum += static_cast<std::uint32_t>(c.g);
                        bSum += static_cast<std::uint32_t>(c.b);
                        aSum += static_cast<std::uint32_t>(c.a);
                    }
                };

                addColor(sourceX + x * 2, sourceY + y * 2);
                addColor(sourceX + x * 2 + 1, sourceY + y * 2);
                addColor(sourceX + x * 2, sourceY + y * 2 + 1);
                addColor(sourceX + x * 2 + 1, sourceY + y * 2 + 1);

                const sf::Color avg(rSum / read, gSum / read, bSum / read, aSum / read);
                result.setPixel(mipX + x, mipY + y, avg);
            }
        }
        sourceX = mipX;
        sourceY = mipY;
        mipY += mipHeight;
        if (mipWidth > 1) { mipWidth /= 2; }
        if (mipHeight > 1) { mipHeight /= 2; }
    }

    return result;
}

sf::IntRect Image::getMipLevelBounds(const glm::u32vec2& size, std::uint32_t level) {
    sf::IntRect bounds(0, 0, size.x, size.y);
    while (level > 0) { bounds = getNextMipLevelBounds(bounds, level--); }
    return bounds;
}

sf::IntRect Image::getNextMipLevelBounds(const sf::IntRect& bounds, std::uint32_t level) {
    if (level == 0) {
        return {bounds.left + bounds.width, bounds.top, bounds.width / 2, bounds.height / 2};
    }
    return {bounds.left, bounds.top + bounds.height, bounds.width / 2, bounds.height / 2};
}

} // namespace vk
} // namespace rc
} // namespace bl
