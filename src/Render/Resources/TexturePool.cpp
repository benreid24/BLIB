#include <BLIB/Render/Resources/TexturePool.hpp>

#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace rc
{
namespace res
{
namespace
{
void generateErrorPattern(sf::Image& img, unsigned int left, unsigned int top, unsigned int width,
                          unsigned int height, unsigned int numBoxes) {
    const unsigned int ErrorBoxWidth  = width / numBoxes;
    const unsigned int ErrorBoxHeight = height / numBoxes;
    for (unsigned int x = left; x < top + width; ++x) {
        for (unsigned int y = top; y < top + height; ++y) {
            const unsigned int xi = x / ErrorBoxWidth;
            const unsigned int yi = y / ErrorBoxHeight;
            if ((xi % 2) == (yi % 2)) { img.setPixel(x, y, sf::Color(230, 66, 245)); }
            else { img.setPixel(x, y, sf::Color(255, 254, 196)); }
        }
    }
}
} // namespace

TexturePool::TexturePool(vk::VulkanState& vs)
: vulkanState(vs)
, textures(Config::MaxTextureCount)
, cubemaps(Config::MaxCubemapCount)
, refCounts(Config::MaxTextureCount)
, freeSlots(Config::MaxTextureCount - Config::MaxRenderTextures - 1)
, freeRtSlots(Config::MaxRenderTextures)
, cubemapRefCounts(Config::MaxCubemapCount)
, cubemapFreeSlots(Config::MaxCubemapCount)
, reverseFileMap(Config::MaxTextureCount - Config::MaxRenderTextures)
, reverseImageMap(Config::MaxTextureCount - Config::MaxRenderTextures) {
    errorTexture.vulkanState = &vs;
    errorTexture.parent      = this;
    for (auto& t : textures) {
        t.parent      = this;
        t.vulkanState = &vs;
    }
    for (auto& t : cubemaps) {
        t.parent      = this;
        t.vulkanState = &vs;
    }
    queuedUpdates.init(vs, [](auto& vec) { vec.reserve(8); });
    toRelease.reserve(64);
}

void TexturePool::init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
                       vk::PerFrame<VkDescriptorSet>& rtDescriptorSets) {
    // create error texture
    constexpr unsigned int ErrorSize    = 1024;
    constexpr unsigned int ErrorBoxSize = 64;
    errorPattern.create(ErrorSize, ErrorSize);
    generateErrorPattern(errorPattern, 0, 0, ErrorSize, ErrorSize, ErrorSize / ErrorBoxSize);
    errorTexture.altImg = &errorPattern;
    errorTexture.createFromContentsAndQueue(
        vk::Texture::Type::Texture2D, vk::TextureFormat::SRGBA32Bit, vk::Sampler::FilteredRepeated);
    vulkanState.transferEngine.executeTransfers();

    // init all textures to error pattern
    for (vk::Texture& txtr : textures) { txtr = errorTexture; }
    for (vk::Texture& txtr : cubemaps) { txtr = errorTexture; }

    // fill descriptor set
    VkDescriptorImageInfo errorInfo{};
    errorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    errorInfo.imageView   = errorTexture.view;
    errorInfo.sampler     = errorTexture.getSamplerHandle();
    std::vector<VkDescriptorImageInfo> imageInfos(
        Config::MaxTextureCount * Config::MaxConcurrentFrames, errorInfo);

    std::array<VkWriteDescriptorSet, 4 * Config::MaxConcurrentFrames> setWrites{};
    unsigned int i = 0;
    const auto visitor =
        [this, &i, &setWrites, &imageInfos](auto& set, std::uint32_t bindIndex, std::uint32_t len) {
            setWrites[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrites[i].descriptorCount = len;
            setWrites[i].dstBinding      = bindIndex;
            setWrites[i].dstArrayElement = 0;
            setWrites[i].dstSet          = set;
            setWrites[i].pImageInfo      = imageInfos.data();
            setWrites[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            ++i;
        };
    descriptorSets.visit(
        [&visitor](auto& set) { visitor(set, TextureArrayBindIndex, Config::MaxTextureCount); });
    rtDescriptorSets.visit(
        [&visitor](auto& set) { visitor(set, TextureArrayBindIndex, Config::MaxTextureCount); });
    descriptorSets.visit(
        [&visitor](auto& set) { visitor(set, CubemapArrayBindIndex, Config::MaxCubemapCount); });
    rtDescriptorSets.visit(
        [&visitor](auto& set) { visitor(set, CubemapArrayBindIndex, Config::MaxCubemapCount); });
    vkUpdateDescriptorSets(vulkanState.device, setWrites.size(), setWrites.data(), 0, nullptr);
}

void TexturePool::cleanup() {
    TextureRef::disableCleanup();
    for (vk::Texture& txtr : textures) {
        if (txtr.view != errorTexture.view) { txtr.cleanup(); }
    }
    errorTexture.cleanup();
}

void TexturePool::releaseUnused() {
    std::unique_lock lock(mutex);
    vkDeviceWaitIdle(vulkanState.device);
    releaseUnusedLocked();
}

void TexturePool::releaseTexture(TextureRef& ref) {
    if (ref.id() == Config::ErrorTextureId) return;

    std::unique_lock lock(mutex);

    if (ref->getType() == vk::Texture::Type::Cubemap) {
        if (cubemapRefCounts[ref.id()] > 1) {
            BL_LOG_WARN << "Releasing cubemap " << ref.id() << " which still has "
                        << cubemapRefCounts[ref.id()] << " references";
        }
    }
    else {
        if (refCounts[ref.id()] > 1) {
            BL_LOG_WARN << "Releasing texture " << ref.id() << " which still has "
                        << refCounts[ref.id()] << " references";
        }
    }

    doRelease(ref.get());
}

void TexturePool::releaseUnusedLocked() {
    for (vk::Texture* t : toRelease) { doRelease(t); }
    toRelease.clear();
}

void TexturePool::doRelease(vk::Texture* texture) {
    if (texture->getType() != vk::Texture::Type::Cubemap) {
        std::uint32_t i = texture - textures.data();
        refCounts[i]    = 0;

        if (i < reverseFileMap.size()) {
            freeSlots.release(i);
            if (reverseFileMap[i]) {
                fileMap.erase(*reverseFileMap[i]);
                reverseFileMap[i] = nullptr;
            }
            if (reverseImageMap[i]) {
                imageMap.erase(reverseImageMap[i]);
                reverseImageMap[i] = nullptr;
            }
        }
        else { freeRtSlots.release(i - reverseFileMap.size()); }
    }
    else {
        std::uint32_t i = texture - cubemaps.data();

        cubemapRefCounts[i] = 0;
        cubemapFreeSlots.release(i);
        if (cubemapFileMap.find(*reverseFileMap[i]) != cubemapFileMap.end()) {
            cubemapFileMap.erase(*cubeMapReverseFileMap[i]);
            cubeMapReverseFileMap[i] = nullptr;
        }
    }
    resetTexture(texture);
}

void TexturePool::queueForRelease(vk::Texture* texture) {
    std::unique_lock lock(mutex);
    toRelease.emplace_back(texture);
}

TextureRef TexturePool::allocateTexture() {
    if (!freeSlots.available()) {
        releaseUnusedLocked();
        if (!freeSlots.available()) { throw std::runtime_error("All texture slots in use!"); }
    }

    const std::uint32_t i = freeSlots.allocate();
    refCounts[i].store(0);
    reverseFileMap[i]  = nullptr;
    reverseImageMap[i] = nullptr;

    return TextureRef{*this, textures[i], i};
}

TextureRef TexturePool::allocateCubemap() {
    if (!cubemapFreeSlots.available()) {
        releaseUnusedLocked();
        if (!cubemapFreeSlots.available()) {
            throw std::runtime_error("All cubemap slots in use!");
        }
    }

    const std::uint32_t i = cubemapFreeSlots.allocate();
    cubemapRefCounts[i].store(0);

    return TextureRef{*this, cubemaps[i], i};
}

TextureRef TexturePool::createTexture(const sf::Image& src, VkFormat format, vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    TextureRef txtr = allocateTexture();
    txtr->altImg    = &src;
    txtr->createFromContentsAndQueue(vk::Texture::Type::Texture2D, format, sampler);
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::createTexture(const glm::u32vec2& size, VkFormat format,
                                      vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    TextureRef txtr = allocateTexture();
    txtr->create(vk::Texture::Type::Texture2D, size, format, sampler);
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::createRenderTexture(const glm::u32vec2& size, VkFormat format,
                                            vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    // allocate id
    if (!freeRtSlots.available()) {
        releaseUnusedLocked();
        if (!freeRtSlots.available()) {
            throw std::runtime_error("All render texture slots in use!");
        }
    }

    const std::uint32_t i = freeRtSlots.allocate() + reverseFileMap.size();
    refCounts[i].store(0);

    // init texture
    TextureRef txtr{*this, textures[i], i};
    txtr->create(vk::Texture::Type::RenderTexture, size, format, sampler);
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const std::string& path, VkFormat format,
                                         vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    auto it = fileMap.find(path);
    if (it != fileMap.end()) {
        auto& t = textures[it->second];
        cancelRelease(&t);
        return TextureRef{*this, t, it->second};
    }

    TextureRef txtr = allocateTexture();
    prepareTextureUpdate(txtr.get(), path);
    it                        = fileMap.try_emplace(path, txtr.id()).first;
    reverseFileMap[txtr.id()] = &it->first;
    txtr->createFromContentsAndQueue(vk::Texture::Type::Texture2D, format, sampler);
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const sf::Image& src, VkFormat format,
                                         vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    auto it = imageMap.find(&src);
    if (it != imageMap.end()) {
        auto& t = textures[it->second];
        cancelRelease(&t);
        return TextureRef{*this, t, it->second};
    }

    TextureRef txtr = allocateTexture();
    prepareTextureUpdate(txtr.get(), src);
    it                         = imageMap.try_emplace(&src, txtr.id()).first;
    reverseImageMap[txtr.id()] = &src;
    txtr->createFromContentsAndQueue(vk::Texture::Type::Texture2D, format, sampler);
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::getOrCreateTexture(const mdl::Texture& texture, TextureRef fallback,
                                           VkFormat format, vk::Sampler sampler) {
    if (texture.isEmbedded()) { return getOrLoadTexture(texture.getEmbedded(), format, sampler); }
    if (texture.getFilePath().empty() ||
        !resource::ResourceManager<sf::Image>::load(texture.getFilePath())) {
        return fallback;
    }
    return getOrLoadTexture(texture.getFilePath(), format, sampler);
}

TextureRef TexturePool::createCubemap(const std::string& right, const std::string& left,
                                      const std::string& top, const std::string& bottom,
                                      const std::string& back, const std::string& front,
                                      VkFormat format, vk::Sampler sampler) {
    auto load = resource::ResourceManager<sf::Image>::load;
    return createCubemap(
        load(right), load(left), load(top), load(bottom), load(back), load(front), format, sampler);
}

TextureRef TexturePool::createCubemap(resource::Ref<sf::Image> right, resource::Ref<sf::Image> left,
                                      resource::Ref<sf::Image> top, resource::Ref<sf::Image> bottom,
                                      resource::Ref<sf::Image> back, resource::Ref<sf::Image> front,
                                      VkFormat format, vk::Sampler sampler) {
    // validate faces
    std::array<sf::Image*, 6> faces = {
        right.get(), left.get(), top.get(), bottom.get(), back.get(), front.get()};
    glm::u32vec2 faceSize(0, 0);
    for (auto& face : faces) {
        if (face && face->getSize().x > 0) {
            if (faceSize.x == 0) {
                faceSize.x = face->getSize().x;
                faceSize.y = face->getSize().y;
            }
            else {
                if (faceSize.x != face->getSize().x || faceSize.y != face->getSize().y) {
                    BL_LOG_ERROR << "Cubemap faces must all be the same size";
                    face = nullptr;
                }
            }
        }
    }
    if (faceSize.x == 0) {
        BL_LOG_ERROR << "No valid cubemap faces provided";
        faceSize = {64, 64};
    }

    std::unique_lock lock(mutex);

    TextureRef cm = allocateCubemap();

    // stitch images into new image
    cm->altImg = &cm->localImage;
    cm->localImage.create(faceSize.x * 6, faceSize.y, sf::Color::Transparent);
    for (std::size_t i = 0; i < 6; ++i) {
        if (faces[i]) { cm->localImage.copy(*faces[i], i * faceSize.x, 0); }
        else {
            generateErrorPattern(cm->localImage, i * faceSize.x, 0, faceSize.x, faceSize.y, 16);
            BL_LOG_WARN << "Cubemap face " << i << " is invalid, using error pattern";
        }
    }

    cm->createFromContentsAndQueue(vk::Texture::Type::Cubemap, format, sampler);
    updateTexture(cm.get());

    return cm;
}

TextureRef TexturePool::createCubemap(resource::Ref<sf::Image> packed, VkFormat format,
                                      vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    TextureRef cm = allocateCubemap();
    if (packed && packed->getSize().x > 0) { cm->transferImg = packed; }
    else { cm->altImg = &errorPattern; }
    cm->createFromContentsAndQueue(vk::Texture::Type::Cubemap, format, sampler);
    updateTexture(cm.get());

    return cm;
}

TextureRef TexturePool::getOrCreateCubemap(const std::string& packed, VkFormat format,
                                           vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    auto it = cubemapFileMap.find(packed);
    if (it != cubemapFileMap.end()) {
        auto& t = cubemaps[it->second];
        cancelRelease(&t);
        return TextureRef{*this, t, it->second};
    }

    TextureRef cm = allocateCubemap();
    prepareTextureUpdate(cm.get(), packed);
    auto fit                       = cubemapFileMap.try_emplace(packed, cm.id()).first;
    cubeMapReverseFileMap[cm.id()] = &fit->first;
    cm->createFromContentsAndQueue(vk::Texture::Type::Cubemap, format, sampler);
    updateTexture(cm.get());

    return cm;
}

TextureRef TexturePool::createCubemap(const sf::Image& packed, VkFormat format,
                                      vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    TextureRef cm  = allocateCubemap();
    cm->localImage = packed;
    cm->altImg     = &cm->localImage;
    cm->createFromContentsAndQueue(vk::Texture::Type::Cubemap, format, sampler);
    updateTexture(cm.get());

    return cm;
}

void TexturePool::onFrameStart(ds::SetWriteHelper& setWriter, VkDescriptorSet currentSet,
                               VkDescriptorSet currentRtSet) {
    if (!queuedUpdates.current().empty()) {
        // prepare descriptor updates before waiting
        setWriter.hintWriteCount(queuedUpdates.current().size() * 2);
        setWriter.hintImageInfoCount(queuedUpdates.current().size() * 2);

        for (unsigned int j = 0; j < queuedUpdates.current().size(); ++j) {
            vk::Texture* texture = queuedUpdates.current()[j];
            const bool isCubemap = texture->getType() == vk::Texture::Type::Cubemap;
            const std::uint32_t bindIndex =
                isCubemap ? CubemapArrayBindIndex : TextureArrayBindIndex;
            const vk::Texture* base = isCubemap ? cubemaps.data() : textures.data();
            const std::uint32_t i   = texture - base;
            const bool isRT         = texture->getType() == vk::Texture::Type::RenderTexture;
            const VkImageView view  = texture->view;

            auto& regularInfo       = setWriter.getNewImageInfo();
            regularInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            regularInfo.imageView   = view;
            regularInfo.sampler     = texture->getSamplerHandle();

            auto& regularWrite           = setWriter.getNewSetWrite(currentSet);
            regularWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            regularWrite.descriptorCount = 1;
            regularWrite.dstBinding      = bindIndex;
            regularWrite.dstArrayElement = i;
            regularWrite.dstSet          = currentSet;
            regularWrite.pImageInfo      = &regularInfo;
            regularWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            // write error texture to rt set if is rt itself
            auto& rtInfo       = setWriter.getNewImageInfo();
            rtInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            rtInfo.imageView   = isRT ? errorTexture.view : view;
            rtInfo.sampler = isRT ? errorTexture.getSamplerHandle() : texture->getSamplerHandle();

            auto& rtWrite           = setWriter.getNewSetWrite(currentRtSet);
            rtWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            rtWrite.descriptorCount = 1;
            rtWrite.dstBinding      = bindIndex;
            rtWrite.dstArrayElement = i;
            rtWrite.pImageInfo      = &rtInfo;
            rtWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }

        queuedUpdates.current().clear();
    }
}

TextureRef TexturePool::getBlankTexture() {
    if (!blankTexture.get()) {
        static sf::Image src;
        if (src.getSize().x == 0) { src.create(2, 2, sf::Color::Transparent); }
        blankTexture = createTexture(src);
    }
    return blankTexture;
}

VkDescriptorSetLayoutBinding TexturePool::getTextureLayoutBinding() const {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorCount    = Config::MaxTextureCount;
    binding.binding            = TextureArrayBindIndex;
    binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.pImmutableSamplers = 0;
    return binding;
}

VkDescriptorSetLayoutBinding TexturePool::getCubemapLayoutBinding() const {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorCount    = Config::MaxCubemapCount;
    binding.binding            = CubemapArrayBindIndex;
    binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.pImmutableSamplers = 0;
    return binding;
}

void TexturePool::prepareTextureUpdate(vk::Texture* texture, const std::string& path) {
    auto img = resource::ResourceManager<sf::Image>::load(path);
    if (img->getSize().x > 0) { texture->transferImg = img; }
    else { texture->altImg = &errorPattern; }
}

void TexturePool::prepareTextureUpdate(vk::Texture* texture, const sf::Image& src) {
    texture->altImg = &src;
}

void TexturePool::updateTexture(vk::Texture* texture) {
    queuedUpdates.visit([texture](auto& vec) { vec.emplace_back(texture); });
}

void TexturePool::resetTexture(vk::Texture* texture) {
    texture->cleanup();
    *texture = errorTexture;
    texture->reset();
    updateTexture(texture);
}

void TexturePool::cancelRelease(vk::Texture* texture) {
    const auto rit = std::find(toRelease.begin(), toRelease.end(), texture);
    if (rit != toRelease.end()) { toRelease.erase(rit); }
}

} // namespace res
} // namespace rc
} // namespace bl
