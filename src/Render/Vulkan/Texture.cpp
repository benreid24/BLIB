#include <BLIB/Render/Vulkan/Texture.hpp>

#include <BLIB/Render/Resources/BindlessTextureArray.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
Texture::Texture()
: altImg(nullptr)
, destPos(0, 0)
, source(0, 0, 0, 0)
, image(nullptr)
, view(nullptr)
, currentLayout(VK_IMAGE_LAYOUT_UNDEFINED) {}

void Texture::createFromContentsAndQueue() {
    const sf::Image& src = altImg ? *altImg : *transferImg;
    create({src.getSize().x, src.getSize().y});
    updateTrans(src);
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::create(const glm::u32vec2& s) {
    updateSize(s);

    vulkanState->createImage(s.x,
                             s.y,
                             DefaultFormat,
                             VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &image,
                             &alloc,
                             &allocInfo);
    view = vulkanState->createImageView(image, DefaultFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    vulkanState->transitionImageLayout(
        image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    currentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    queueTransfer(SyncRequirement::Immediate);
}

void Texture::update(const sf::Image& content, const glm::u32vec2& dp, const sf::IntRect& s) {
    altImg  = &content;
    destPos = dp;
    source  = s;
    updateTrans(content);
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::update(const resource::Ref<sf::Image>& content, const glm::u32vec2& dp,
                     const sf::IntRect& s) {
    transferImg = content;
    destPos     = dp;
    source      = s;
    updateTrans(*content);
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::resize(const glm::u32vec2& s) {
    // cache data needed to delete original image
    VkImage oldImage = image;

    // queue deletion of original resources
    vulkanState->cleanupManager.add(
        [vulkanState = vulkanState, oldImage, oldAlloc = alloc, oldView = view]() {
            vkDestroyImageView(vulkanState->device, oldView, nullptr);
            vmaDestroyImage(vulkanState->vmaAllocator, oldImage, oldAlloc);
        });

    // transition original image to transfer source layout
    vulkanState->transitionImageLayout(
        image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    currentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    // create new image
    const glm::u32vec2 oldSize = rawSize();
    create(s);

    // copy from old to new
    auto cb = vulkanState->sharedCommandPool.createBuffer();

    VkImageCopy copyInfo{};
    copyInfo.extent.width                  = std::min(oldSize.x, s.x);
    copyInfo.extent.height                 = std::min(oldSize.y, s.y);
    copyInfo.extent.depth                  = 1;
    copyInfo.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copyInfo.srcSubresource.mipLevel       = 0;
    copyInfo.srcSubresource.baseArrayLayer = 0;
    copyInfo.srcSubresource.layerCount     = 1;
    copyInfo.dstSubresource                = copyInfo.srcSubresource;

    vkCmdCopyImage(cb,
                   oldImage,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &copyInfo);

    cb.submit();

    updateDescriptors();
}

void Texture::executeTransfer(VkCommandBuffer cb, tfr::TransferContext& engine) {
    const auto transitionLayout = [this, cb, &engine]() {
        VkImageMemoryBarrier barrier{};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
        engine.registerImageBarrier(barrier);
        currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    };

    // copy image contents if required
    if (altImg || transferImg) {
        const sf::Image& src = altImg ? *altImg : *transferImg;

        bool fullImage = false;
        if (source.width == 0 || source.height == 0) {
            fullImage     = true;
            source.left   = 0;
            source.top    = 0;
            source.width  = src.getSize().x;
            source.height = src.getSize().y;
        }

        // create staging buffer
        const VkDeviceSize stageSize = source.width * source.height * 4;
        VkBuffer stagingBuffer;
        void* data;
        engine.createTemporaryStagingBuffer(stageSize, stagingBuffer, &data);
        if (!fullImage) {
            for (int x = source.left; x < source.left + source.width; ++x) {
                for (int y = source.top; y < source.top + source.height; ++y) {
                    const std::ptrdiff_t offset = (x + y * source.width) * 4;
                    std::uint8_t* d             = static_cast<std::uint8_t*>(data) + offset;
                    const std::uint8_t* s       = src.getPixelsPtr() + offset;
                    std::memcpy(d, s, 4);
                }
            }
        }
        else { std::memcpy(data, src.getPixelsPtr(), stageSize); }

        // transition to transfer dst prior to copy
        vulkanState->transitionImageLayout(
            cb, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // issue copy command
        VkBufferImageCopy copyInfo{};
        copyInfo.bufferOffset                    = 0;
        copyInfo.bufferRowLength                 = 0;
        copyInfo.bufferImageHeight               = 0;
        copyInfo.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        copyInfo.imageSubresource.mipLevel       = 0;
        copyInfo.imageSubresource.baseArrayLayer = 0;
        copyInfo.imageSubresource.layerCount     = 1;
        copyInfo.imageOffset.x                   = destPos.x;
        copyInfo.imageOffset.y                   = destPos.y;
        copyInfo.imageExtent.width               = source.width;
        copyInfo.imageExtent.height              = source.height;
        copyInfo.imageExtent.depth               = 1;
        vkCmdCopyBufferToImage(
            cb, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);

        // insert pipeline barrier
        transitionLayout();

        // cleanup
        transferImg.release();
        altImg        = nullptr;
        destPos.x     = 0;
        destPos.y     = 0;
        source.left   = 0;
        source.top    = 0;
        source.width  = 0;
        source.height = 0;
    }
    else if (currentLayout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) { transitionLayout(); }
}

void Texture::cleanup() {
    vkDestroyImageView(vulkanState->device, view, nullptr);
    vmaDestroyImage(vulkanState->vmaAllocator, image, alloc);

    cancelQueuedTransfer();
}

void Texture::reset() {
    transferImg.release();
    altImg        = nullptr;
    destPos.x     = 0;
    destPos.y     = 0;
    source.left   = 0;
    source.top    = 0;
    source.width  = 0;
    source.height = 0;
}

void Texture::updateTrans(const sf::Image& content) {
    const std::uint32_t s      = content.getSize().x * content.getSize().y;
    const std::uint32_t thresh = s / 10;
    std::uint32_t t            = 0;

    // jump directly from alpha to alpha
    const std::uint8_t* end = content.getPixelsPtr() + s * 4;
    for (const std::uint8_t* a = content.getPixelsPtr() + 3; a < end; a += 4) {
        if (*a > 0 || *a < 255) {
            ++t;
            if (t >= thresh) {
                updateTransparency(true);
                return;
            }
        }
    }

    updateTransparency(false);
}

} // namespace vk
} // namespace rc
} // namespace bl
