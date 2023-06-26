#include <BLIB/Render/Vulkan/Texture.hpp>

#include <BLIB/Render/Resources/BindlessTextureArray.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
Texture::Texture()
: parent(nullptr)
, destPos(0, 0)
, source(0, 0, 0, 0)
, image(nullptr)
, view(nullptr)
, sampler(nullptr)
, sizeRaw(0, 0)
, sizeF(0.f, 0.f)
, needsCleanup(false) {}

void Texture::createFromContentsAndQueue() {
    const sf::Image& src = altImg ? *altImg : *transferImg;
    create({src.getSize().x, src.getSize().y}, DefaultFormat, 0);
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::create(const glm::u32vec2& s, VkFormat f, VkImageUsageFlags u) {
    usage   = u;
    format  = f;
    sizeRaw = s;
    sizeF.x = static_cast<float>(sizeRaw.x);
    sizeF.y = static_cast<float>(sizeRaw.y);

    vulkanState->createImage(s.x,
                             s.y,
                             format,
                             VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | usage,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &image,
                             &alloc,
                             &allocInfo);
    view = vulkanState->createImageView(image, format, VK_IMAGE_ASPECT_COLOR_BIT);

    if ((u & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == 0) {
        vulkanState->transitionImageLayout(
            image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    }
    else {
        vulkanState->transitionImageLayout(
            image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}

void Texture::setSampler(VkSampler s) {
    sampler = s;
    parent->updateTexture(this);
}

void Texture::update(const sf::Image& content, const glm::u32vec2& dp, const sf::IntRect& s) {
    altImg  = &content;
    destPos = dp;
    source  = s;
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::update(const resource::Ref<sf::Image>& content, const glm::u32vec2& dp,
                     const sf::IntRect& s) {
    transferImg = content;
    destPos     = dp;
    source      = s;
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::ensureSize(const glm::u32vec2& s) {
    if (s.x <= sizeRaw.x && s.y <= sizeRaw.y) return;

    // cache data needed to delete original image
    queueCleanup();

    // transition original image to transfer source layout
    vulkanState->transitionImageLayout(
        image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    // create new image
    const glm::u32vec2 oldSize = sizeRaw;
    create(s, format, usage);

    // copy from old to new
    VkCommandBuffer cb = vulkanState->beginSingleTimeCommands();

    VkImageCopy copyInfo{};
    copyInfo.extent.width                  = oldSize.x;
    copyInfo.extent.height                 = oldSize.y;
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

    vulkanState->endSingleTimeCommands(cb);

    // clean up old image once transfer is complete via transfer engine
    queueTransfer(SyncRequirement::DeviceIdle);
}

void Texture::executeTransfer(VkCommandBuffer cb, tfr::TransferContext& engine) {
    // clean up origin texture if required
    if (needsCleanup) {
        needsCleanup = false;
        vkDestroyImageView(vulkanState->device, oldView, nullptr);
        vmaDestroyImage(vulkanState->vmaAllocator, oldImage, oldAlloc);
        parent->updateTexture(this);
    }

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

        // issue copy command
        VkBufferImageCopy copyInfo{};
        copyInfo.bufferOffset                    = 0;
        copyInfo.bufferRowLength                 = 0;
        copyInfo.bufferImageHeight               = 0;
        copyInfo.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        copyInfo.imageSubresource.mipLevel       = 0;
        copyInfo.imageSubresource.baseArrayLayer = 0;
        copyInfo.imageSubresource.layerCount     = 1;
        copyInfo.imageOffset                     = VkOffset3D(destPos.x, destPos.y, 0);
        copyInfo.imageExtent                     = VkExtent3D(source.width, source.height, 1);
        vkCmdCopyBufferToImage(
            cb, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);

        // insert pipeline barrier
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
}

void Texture::cleanup() {
    vkDestroyImageView(vulkanState->device, view, nullptr);
    vmaDestroyImage(vulkanState->vmaAllocator, image, alloc);
}

void Texture::queueCleanup() {
    needsCleanup = true;
    oldImage     = image;
    oldAlloc     = alloc;
    oldView      = view;
}

} // namespace vk
} // namespace gfx
} // namespace bl
