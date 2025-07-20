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

ImageOptions::Type mapType(Texture::Type type) {
    switch (type) {
    case Texture::Type::RenderTexture:
        return ImageOptions::Type::Image2D;

    case Texture::Type::Cubemap:
    case Texture::Type::Texture2D:
    default:
        return static_cast<ImageOptions::Type>(type);
    }
}

} // namespace

Texture::Texture()
: parent(nullptr)
, createOptions()
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
    createOptions.sampler = s;
    updateDescriptors();
}

void Texture::ensureSize(const glm::u32vec2& s) {
    if (s.x <= rawSize().x && s.y <= rawSize().y) return;

    resize(s);
}

void Texture::updateDescriptors() { parent->updateTexture(this); }

void Texture::createFromContentsAndQueue(Type type, const TextureOptions& options) {
    const sf::Image& src = altImg ? *altImg : *transferImg;
    glm::vec2 createSize = {src.getSize().x, src.getSize().y};
    if (type == Type::Cubemap) { createSize.y /= 6; }
    create(type, createSize, options);
    updateTrans(src);
    queueTransfer(SyncRequirement::Immediate);
}

void Texture::create(Type t, const glm::u32vec2& s, const TextureOptions& options) {
    type          = t;
    createOptions = options;

    image.create(*vulkanState,
                 {.type   = mapType(type),
                  .format = createOptions.format,
                  .usage  = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                           VK_IMAGE_USAGE_TRANSFER_SRC_BIT | getExtraUsage(type),
                  .extent    = {s.x, s.y},
                  .aspect    = VK_IMAGE_ASPECT_COLOR_BIT,
                  .mipLevels = createOptions.genMipmaps ?
                                   Image::computeMipLevelsForGeneration(s.x, s.y) :
                                   1});
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
    currentView = image.getView();
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
        barrier.subresourceRange.levelCount     = image.getLevelCount();
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = image.getLayerCount();
        barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
        engine.registerImageBarrier(barrier);
        image.notifyNewLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    };

    const auto copyImageToStaging = [](const sf::Image& src, void* dst, const sf::IntRect& region) {
        for (int x = region.left; x < region.left + region.width; ++x) {
            for (int y = region.top; y < region.top + region.height; ++y) {
                const std::ptrdiff_t offset = (x + y * src.getSize().x) * sizeof(sf::Color);
                std::uint8_t* d             = static_cast<std::uint8_t*>(dst) + offset;
                const std::uint8_t* s       = src.getPixelsPtr() + offset;
                std::memcpy(d, s, sizeof(sf::Color));
            }
        }
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
        if (!fullImage) { copyImageToStaging(src, data, source); }
        else { std::memcpy(data, src.getPixelsPtr(), stageSize); }

        // transition to transfer dst prior to copy
        image.transitionLayout(cb, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, true);

        // TODO - support loading existing mip maps
        // issue copy command
        VkDeviceSize layerSize = image.getSize().width * image.getSize().height * sizeof(sf::Color);
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

        bool needsTransferToShaderSrcLayout = true;

        // generate mip maps if requested
        if (createOptions.genMipmaps) {
            if (image.canGenMipMapsOnGpu()) {
                image.genMipMapsOnGpu(cb, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                needsTransferToShaderSrcLayout = false;
            }
            else if (image.getLevelCount() > 1) {
                const std::uint32_t mipLevels = image.getLevelCount();
                std::vector<VkBufferImageCopy> copyInfos;
                copyInfos.resize(mipLevels - 1, VkBufferImageCopy{});

                // pre-pass to determine total size
                std::size_t totalSize = 0;
                sf::IntRect bounds =
                    Image::getMipLevelBounds({src.getSize().x, src.getSize().y}, 1);
                for (unsigned int i = 1; i < mipLevels; ++i) {
                    totalSize += bounds.width * bounds.height * sizeof(sf::Color);
                    bounds = Image::getNextMipLevelBounds(bounds, i);
                }

                // get one combined staging buffer
                VkBuffer mipStaging;
                void* stagingDst = nullptr;
                engine.createTemporaryStagingBuffer(totalSize, mipStaging, &stagingDst);

                // gen on cpu
                sf::Image mipSrc = Image::genMipMapsOnCpu(src);

                // copy mip levels
                std::uint32_t baseOffset = 0;
                bounds = Image::getMipLevelBounds({src.getSize().x, src.getSize().y}, 1);
                for (unsigned int i = 1; i < mipLevels; ++i) {
                    // copy from staging to mip image
                    auto& copy              = copyInfos[i - 1];
                    copy.bufferOffset       = baseOffset;
                    copy.bufferImageHeight  = 0;
                    copy.bufferRowLength    = 0;
                    copy.imageExtent.width  = bounds.width;
                    copy.imageExtent.height = bounds.height;
                    copy.imageExtent.depth  = 1;
                    copy.imageOffset.x = copy.imageOffset.y = copy.imageOffset.z = 0;
                    copy.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                    copy.imageSubresource.mipLevel       = i;
                    copy.imageSubresource.baseArrayLayer = 0;
                    copy.imageSubresource.layerCount     = 1;

                    // copy from cpu to staging
                    const std::uint32_t copySize = bounds.width * bounds.height * sizeof(sf::Color);
                    const std::uint32_t srcOffset =
                        (bounds.top * mipSrc.getSize().x + bounds.left) * sizeof(sf::Color);
                    void* copyDst = static_cast<char*>(stagingDst) + baseOffset;
                    std::memcpy(copyDst, mipSrc.getPixelsPtr() + srcOffset, copySize);

                    baseOffset += copySize;
                    bounds = Image::getNextMipLevelBounds(bounds, i);
                }

                // issue copy commands
                vkCmdCopyBufferToImage(cb,
                                       mipStaging,
                                       image.getImage(),
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       copyInfos.size(),
                                       copyInfos.data());
            }
        }

        // insert pipeline barrier if required
        if (needsTransferToShaderSrcLayout) { transitionLayout(); }

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
    // TODO - need mip-aware samplers
    return vulkanState->samplerCache.getSampler(createOptions.sampler);
}

} // namespace vk
} // namespace rc
} // namespace bl
