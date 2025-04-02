#include <BLIB/Render/Resources/TexturePool.hpp>

#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace rc
{
namespace res
{
TexturePool::TexturePool(vk::VulkanState& vs)
: vulkanState(vs)
, textures(Config::MaxTextureCount)
, refCounts(Config::MaxTextureCount)
, freeSlots(Config::MaxTextureCount - Config::MaxRenderTextures - 1)
, freeRtSlots(Config::MaxRenderTextures)
, reverseFileMap(Config::MaxTextureCount - Config::MaxRenderTextures)
, reverseImageMap(Config::MaxTextureCount - Config::MaxRenderTextures) {
    errorTexture.vulkanState = &vs;
    errorTexture.parent      = this;
    for (auto& t : textures) {
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
    for (unsigned int x = 0; x < ErrorSize; ++x) {
        for (unsigned int y = 0; y < ErrorSize; ++y) {
            const unsigned int xi = x / ErrorBoxSize;
            const unsigned int yi = y / ErrorBoxSize;
            if ((xi % 2) == (yi % 2)) { errorPattern.setPixel(x, y, sf::Color(230, 66, 245)); }
            else { errorPattern.setPixel(x, y, sf::Color(255, 254, 196)); }
        }
    }
    errorTexture.altImg  = &errorPattern;
    errorTexture.sampler = vulkanState.samplerCache.filteredRepeated();
    errorTexture.format  = vk::TextureFormat::SRGBA32Bit;
    errorTexture.createFromContentsAndQueue();
    vulkanState.transferEngine.executeTransfers();

    // init all textures to error pattern
    for (vk::Texture& txtr : textures) { txtr = errorTexture; }

    // fill descriptor set
    const std::size_t len = textures.size();
    std::vector<VkDescriptorImageInfo> imageInfos(len * Config::MaxConcurrentFrames,
                                                  VkDescriptorImageInfo{});
    for (unsigned int fi = 0; fi < Config::MaxConcurrentFrames; ++fi) {
        std::size_t infoIndex = len * fi;

        for (unsigned int i = 0; i < textures.size(); ++i, ++infoIndex) {
            auto& info       = imageInfos[infoIndex];
            info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            info.imageView   = textures[i].view;
            info.sampler     = textures[i].getSampler();
        }
    }

    std::array<VkWriteDescriptorSet, 2 * Config::MaxConcurrentFrames> setWrites{};
    unsigned int i     = 0;
    unsigned int fi    = 0;
    const auto visitor = [this, &i, &fi, len, &setWrites, &imageInfos](auto& set) {
        setWrites[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrites[i].descriptorCount = len;
        setWrites[i].dstBinding      = TextureArrayBindIndex;
        setWrites[i].dstArrayElement = 0;
        setWrites[i].dstSet          = set;
        setWrites[i].pImageInfo      = imageInfos.data() + fi * len;
        setWrites[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        ++i;
        ++fi;
    };
    descriptorSets.visit(visitor);
    fi = 0;
    rtDescriptorSets.visit(visitor);
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

void TexturePool::releaseTexture(const TextureRef& ref) {
    if (ref.id() == Config::ErrorTextureId) return;

    std::unique_lock lock(mutex);

    if (refCounts[ref.id()] > 1) {
        BL_LOG_WARN << "Releasing texture " << ref.id() << " which still has "
                    << refCounts[ref.id()] << " references";
    }

    doRelease(ref.id());
}

void TexturePool::releaseUnusedLocked() {
    for (std::uint32_t i : toRelease) { doRelease(i); }
    toRelease.clear();
}

void TexturePool::doRelease(std::uint32_t i) {
    refCounts[i] = 0;

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

    resetTexture(i);
}

void TexturePool::queueForRelease(std::uint32_t i) {
    std::unique_lock lock(mutex);
    toRelease.emplace_back(i);
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

TextureRef TexturePool::createTexture(const sf::Image& src, VkFormat format, vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    TextureRef txtr = allocateTexture();
    auto& t         = static_cast<vk::Texture&>(*txtr.get());
    t.altImg        = &src;
    txtr->sampler   = vulkanState.samplerCache.getSampler(sampler);
    txtr->format    = format;
    t.createFromContentsAndQueue();
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::createTexture(const glm::u32vec2& size, VkFormat format,
                                      vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    TextureRef txtr = allocateTexture();
    txtr->sampler   = vulkanState.samplerCache.getSampler(sampler);
    txtr->format    = format;
    txtr->create(size);
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
    txtr->sampler = vulkanState.samplerCache.getSampler(sampler);
    txtr->format  = format;
    txtr->create(size, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const std::string& path, VkFormat format,
                                         vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    auto it = fileMap.find(path);
    if (it != fileMap.end()) {
        const auto rit = std::find(toRelease.begin(), toRelease.end(), it->second);
        if (rit != toRelease.end()) { toRelease.erase(rit); }
        return TextureRef{*this, textures[it->second], it->second};
    }

    TextureRef txtr = allocateTexture();
    prepareTextureUpdate(txtr.id(), path);
    it                        = fileMap.try_emplace(path, txtr.id()).first;
    reverseFileMap[txtr.id()] = &it->first;
    txtr->sampler             = vulkanState.samplerCache.getSampler(sampler);
    txtr->format              = format;
    static_cast<vk::Texture&>(*txtr.get()).createFromContentsAndQueue();
    updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const sf::Image& src, VkFormat format,
                                         vk::Sampler sampler) {
    std::unique_lock lock(mutex);

    auto it = imageMap.find(&src);
    if (it != imageMap.end()) {
        const auto rit = std::find(toRelease.begin(), toRelease.end(), it->second);
        if (rit != toRelease.end()) { toRelease.erase(rit); }
        return TextureRef{*this, textures[it->second], it->second};
    }

    TextureRef txtr = allocateTexture();
    prepareTextureUpdate(txtr.id(), src);
    it                         = imageMap.try_emplace(&src, txtr.id()).first;
    reverseImageMap[txtr.id()] = &src;
    txtr->sampler              = vulkanState.samplerCache.getSampler(sampler);
    txtr->format               = format;
    static_cast<vk::Texture&>(*txtr.get()).createFromContentsAndQueue();
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

void TexturePool::onFrameStart(ds::SetWriteHelper& setWriter, VkDescriptorSet currentSet,
                               VkDescriptorSet currentRtSet) {
    if (!queuedUpdates.current().empty()) {
        // prepare descriptor updates before waiting
        setWriter.hintWriteCount(queuedUpdates.current().size() * 2);
        setWriter.hintImageInfoCount(queuedUpdates.current().size() * 2);

        for (unsigned int j = 0; j < queuedUpdates.current().size(); ++j) {
            vk::Texture* texture   = queuedUpdates.current()[j];
            const std::uint32_t i  = texture - textures.data();
            const bool isRT        = i >= FirstRenderTextureId;
            const VkImageView view = texture->view;

            auto& regularInfo       = setWriter.getNewImageInfo();
            regularInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            regularInfo.imageView   = view;
            regularInfo.sampler     = texture->getSampler();

            auto& regularWrite           = setWriter.getNewSetWrite(currentSet);
            regularWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            regularWrite.descriptorCount = 1;
            regularWrite.dstBinding      = TextureArrayBindIndex;
            regularWrite.dstArrayElement = i;
            regularWrite.dstSet          = currentSet;
            regularWrite.pImageInfo      = &regularInfo;
            regularWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            // write error texture to rt set if is rt itself
            auto& rtInfo       = setWriter.getNewImageInfo();
            rtInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            rtInfo.imageView   = isRT ? errorTexture.view : view;
            rtInfo.sampler     = isRT ? errorTexture.getSampler() : texture->getSampler();

            auto& rtWrite           = setWriter.getNewSetWrite(currentRtSet);
            rtWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            rtWrite.descriptorCount = 1;
            rtWrite.dstBinding      = TextureArrayBindIndex;
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

VkDescriptorSetLayoutBinding TexturePool::getLayoutBinding() const {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorCount    = Config::MaxTextureCount;
    binding.binding            = TextureArrayBindIndex;
    binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.pImmutableSamplers = 0;
    return binding;
}

void TexturePool::prepareTextureUpdate(std::uint32_t i, const std::string& path) {
    auto img = resource::ResourceManager<sf::Image>::load(path);
    if (img->getSize().x > 0) { textures[i].transferImg = img; }
    else { textures[i].altImg = &errorPattern; }
}

void TexturePool::prepareTextureUpdate(std::uint32_t i, const sf::Image& src) {
    textures[i].altImg = &src;
}

void TexturePool::updateTexture(vk::TextureBase* texture) {
    queuedUpdates.visit(
        [texture](auto& vec) { vec.emplace_back(static_cast<vk::Texture*>(texture)); });
}

void TexturePool::resetTexture(std::uint32_t i) {
    vk::Texture* texture = &textures[i];
    texture->cleanup();
    *texture = errorTexture;
    texture->reset();
    updateTexture(texture);
}

} // namespace res
} // namespace rc
} // namespace bl
