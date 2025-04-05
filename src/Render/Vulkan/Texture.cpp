#include <BLIB/Render/Vulkan/Texture.hpp>

#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
namespace
{
std::uint32_t getLayerCount(Texture::Type type) {
    switch (type) {
    case Texture::Type::Cubemap:
        return 6;
    case Texture::Type::RenderTexture:
    case Texture::Type::Texture2D:
    default:
        return 1;
    }
}

VkImageUsageFlags getExtraUsage(Texture::Type type) {
    switch (type) {
    case Texture::Type::RenderTexture:
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    case Texture::Type::Cubemap:
    case Texture::Type::Texture2D:
    default:
        return 0;
    }
}

VkImageCreateFlags getCreateFlags(Texture::Type type) {
    switch (type) {
    case Texture::Type::Cubemap:
        return VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    case Texture::Type::RenderTexture:
    case Texture::Type::Texture2D:
    default:
        return 0;
    }
}

VkImageViewType getViewType(Texture::Type type) {
    switch (type) {
    case Texture::Type::Cubemap:
        return VK_IMAGE_VIEW_TYPE_CUBE;
    case Texture::Type::RenderTexture:
    case Texture::Type::Texture2D:
    default:
        return VK_IMAGE_VIEW_TYPE_2D;
    }
}
} // namespace

Texture::Texture()
: parent(nullptr)
, sampler(Sampler::FilteredRepeated)
, sizeRaw(0, 0)
, hasTransparency(false)
, altImg(nullptr)
, destPos(0, 0)
, source(0, 0, 0, 0)
, image(nullptr)
, view(nullptr)
, currentLayout(VK_IMAGE_LAYOUT_UNDEFINED) {}

glm::vec2 Texture::convertCoord(const glm::vec2& src) const {
    // TODO - texture atlasing at the renderer level
    return src;
}

glm::vec2 Texture::normalizeAndConvertCoord(const glm::vec2& src) const {
    return convertCoord(src / size());
}

void Texture::setSampler(Sampler s) {
    sampler = s;
    updateDescriptors();
}

void Texture::ensureSize(const glm::u32vec2& s) {
    if (s.x <= rawSize().x && s.y <= rawSize().y) return;

    resize(s);
}

void Texture::updateDescriptors() { parent->updateTexture(this); }

void Texture::createFromContentsAndQueue(Type type, VkFormat format, Sampler sampler) {
    const sf::Image& src = altImg ? *altImg : *transferImg;
    create(type, {src.getSize().x, src.getSize().y}, format, sampler);
    updateTrans(src);
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::create(Type t, const glm::u32vec2& s, VkFormat fmt, Sampler smplr) {
    type    = t;
    sampler = smplr;
    format  = fmt;
    sizeRaw = s;
    if (type == Type::Cubemap) { sizeRaw.x /= 6; }

    vulkanState->createImage(s.x,
                             s.y,
                             getLayerCount(type),
                             getFormat(),
                             VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | getExtraUsage(type),
                             getCreateFlags(type),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &image,
                             &alloc,
                             &allocInfo);
    view = vulkanState->createImageView(
        image, getFormat(), VK_IMAGE_ASPECT_COLOR_BIT, getLayerCount(type), getViewType(type));

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
    create(type, s, format, sampler);

    // copy from old to new
    auto cb = vulkanState->sharedCommandPool.createBuffer();

    VkImageCopy copyInfo{};
    copyInfo.extent.width                  = std::min(oldSize.x, s.x);
    copyInfo.extent.height                 = std::min(oldSize.y, s.y);
    copyInfo.extent.depth                  = 1;
    copyInfo.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copyInfo.srcSubresource.mipLevel       = 0;
    copyInfo.srcSubresource.baseArrayLayer = 0;
    copyInfo.srcSubresource.layerCount     = getLayerCount(type);
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
        barrier.subresourceRange.layerCount     = getLayerCount(type);
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
        VkDeviceSize layerSize = sizeRaw.x * sizeRaw.y * 4;
        VkBufferImageCopy copyInfos[6]{};
        for (unsigned int face = 0; face < getLayerCount(type); ++face) {
            auto& copyInfo                           = copyInfos[face];
            copyInfo.bufferOffset                    = face * layerSize;
            copyInfo.bufferRowLength                 = 0;
            copyInfo.bufferImageHeight               = 0;
            copyInfo.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            copyInfo.imageSubresource.mipLevel       = 0;
            copyInfo.imageSubresource.baseArrayLayer = face;
            copyInfo.imageSubresource.layerCount     = 1;
            copyInfo.imageOffset.x                   = destPos.x;
            copyInfo.imageOffset.y                   = destPos.y;
            copyInfo.imageExtent.width               = source.width;
            copyInfo.imageExtent.height              = source.height;
            copyInfo.imageExtent.depth               = 1;
        }
        vkCmdCopyBufferToImage(cb,
                               stagingBuffer,
                               image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               getLayerCount(type),
                               copyInfos);

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
                hasTransparency = true;
                return;
            }
        }
    }

    hasTransparency = false;
}

VkSampler Texture::getSamplerHandle() const {
    return vulkanState->samplerCache.getSampler(sampler);
}

} // namespace vk
} // namespace rc
} // namespace bl
