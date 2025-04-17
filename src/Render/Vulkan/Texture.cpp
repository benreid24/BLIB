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

Image::Type mapType(Texture::Type type) {
    switch (type) {
    case Texture::Type::RenderTexture:
        return Image::Type::Image2D;

    case Texture::Type::Cubemap:
    case Texture::Type::Texture2D:
    default:
        return static_cast<Image::Type>(type);
    }
}

} // namespace

Texture::Texture()
: parent(nullptr)
, sampler(Sampler::FilteredRepeated)
, hasTransparency(false)
, altImg(nullptr)
, destPos(0, 0)
, source(0, 0, 0, 0) {}

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
    glm::vec2 createSize = {src.getSize().x, src.getSize().y};
    if (type == Type::Cubemap) { createSize.y /= 6; }
    create(type, createSize, format, sampler);
    updateTrans(src);
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::create(Type t, const glm::u32vec2& s, VkFormat fmt, Sampler smplr) {
    type    = t;
    sampler = smplr;

    image.create(*vulkanState,
                 mapType(type),
                 fmt,
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                     VK_IMAGE_USAGE_TRANSFER_SRC_BIT | getExtraUsage(type),
                 {s.x, s.y},
                 VK_IMAGE_ASPECT_COLOR_BIT);
    image.transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    currentView = image.getView();

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
    image.resize(s, true);
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
        barrier.image                           = image.getImage();
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = image.getLayerCount();
        barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
        engine.registerImageBarrier(barrier);
        image.notifyNewLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
            source.height /= image.getLayerCount();
        }

        // create staging buffer
        const VkDeviceSize stageSize = source.width * source.height * 4 * image.getLayerCount();
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
        image.transitionLayout(cb, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, true);

        // issue copy command
        VkDeviceSize layerSize = image.getSize().width * image.getSize().height * 4;
        VkBufferImageCopy copyInfos[6]{};
        for (unsigned int face = 0; face < image.getLayerCount(); ++face) {
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
                               image.getImage(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               image.getLayerCount(),
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
    else if (image.getCurrentLayout() != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        transitionLayout();
    }
}

void Texture::cleanup() {
    image.destroy();
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
