#include <BLIB/Render/Resources/BindlessTextureArray.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Vulkan/TextureFormat.hpp>

namespace bl
{
namespace rc
{
namespace res
{
BindlessTextureArray::BindlessTextureArray(vk::VulkanState& vs, std::uint32_t size,
                                           std::uint32_t bind)
: bindIndex(bind)
, firstRtId(size - Config::MaxRenderTextures)
, vulkanState(vs)
, textures(size) {
    errorTexture.vulkanState = &vs;
    errorTexture.parent      = this;
    for (auto& t : textures) {
        t.parent      = this;
        t.vulkanState = &vs;
    }
    queuedUpdates.init(vs, [](auto& vec) { vec.reserve(8); });
}

void BindlessTextureArray::init(vk::PerFrame<VkDescriptorSet>& ds,
                                vk::PerFrame<VkDescriptorSet>& rtds) {
    // ensure an error pattern exists
    if (errorPattern.getSize().x == 0) { errorPattern.create(32, 32, sf::Color(245, 66, 242)); }

    // init error pattern texture
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
        setWrites[i].dstBinding      = bindIndex;
        setWrites[i].dstArrayElement = 0;
        setWrites[i].dstSet          = set;
        setWrites[i].pImageInfo      = imageInfos.data() + fi * len;
        setWrites[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        ++i;
        ++fi;
    };
    ds.visit(visitor);
    fi = 0;
    rtds.visit(visitor);
    vkUpdateDescriptorSets(vulkanState.device, setWrites.size(), setWrites.data(), 0, nullptr);
}

void BindlessTextureArray::cleanup() {
    for (vk::Texture& txtr : textures) {
        if (txtr.view != errorTexture.view) { txtr.cleanup(); }
    }
    errorTexture.cleanup();
}

void BindlessTextureArray::prepareTextureUpdate(std::uint32_t i, const std::string& path) {
    auto img = resource::ResourceManager<sf::Image>::load(path);
    if (img->getSize().x > 0) { textures[i].transferImg = img; }
    else { textures[i].altImg = &errorPattern; }
}

void BindlessTextureArray::prepareTextureUpdate(std::uint32_t i, const sf::Image& src) {
    textures[i].altImg = &src;
}

void BindlessTextureArray::updateTexture(vk::TextureBase* texture) {
    queuedUpdates.visit(
        [texture](auto& vec) { vec.emplace_back(static_cast<vk::Texture*>(texture)); });
}

void BindlessTextureArray::resetTexture(std::uint32_t i) {
    vk::Texture* texture = &textures[i];
    texture->cleanup();
    *texture = errorTexture;
    texture->reset();
    updateTexture(texture);
}

void BindlessTextureArray::commitDescriptorUpdates(ds::SetWriteHelper& setWriter,
                                                   VkDescriptorSet currentSet,
                                                   VkDescriptorSet currentRtSet) {
    if (!queuedUpdates.current().empty()) {
        // prepare descriptor updates before waiting
        setWriter.hintWriteCount(queuedUpdates.current().size() * 2);
        setWriter.hintImageInfoCount(queuedUpdates.current().size() * 2);

        for (unsigned int j = 0; j < queuedUpdates.current().size(); ++j) {
            vk::Texture* texture   = queuedUpdates.current()[j];
            const std::uint32_t i  = texture - textures.data();
            const bool isRT        = i >= firstRtId;
            const VkImageView view = texture->view;

            auto& regularInfo       = setWriter.getNewImageInfo();
            regularInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            regularInfo.imageView   = view;
            regularInfo.sampler     = texture->getSampler();

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
            rtInfo.sampler     = isRT ? errorTexture.getSampler() : texture->getSampler();

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

} // namespace res
} // namespace rc
} // namespace bl
