#include <BLIB/Render/Resources/BindlessTextureArray.hpp>

#include <BLIB/Render/Config.hpp>

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
, textures(size - Config::MaxRenderTextures)
, renderTextures(Config::MaxRenderTextures) {
    errorTexture.vulkanState = &vs;
    errorTexture.parent      = this;
    for (auto& t : textures) {
        t.parent      = this;
        t.vulkanState = &vs;
    }
    for (auto& t : renderTextures) {
        t.parent      = this;
        t.vulkanState = &vs;
    }
    queuedUpdates.init(vs, [](auto& vec) { vec.reserve(8); });
}

void BindlessTextureArray::init(vk::PerFrame<VkDescriptorSet>& ds,
                                vk::PerFrame<VkDescriptorSet>& rtds) {
    descriptorSets   = &ds;
    rtDescriptorSets = &rtds;

    // ensure an error pattern exists
    if (errorPattern.getSize().x == 0) { errorPattern.create(32, 32, sf::Color(245, 66, 242)); }

    // init error pattern texture
    errorTexture.altImg = &errorPattern;
    errorTexture.createFromContentsAndQueue();
    errorTexture.sampler = vulkanState.samplerCache.filteredRepeated();
    vulkanState.transferEngine.executeTransfers();

    // init all textures to error pattern
    for (vk::Texture& txtr : textures) { txtr = errorTexture; }
    for (vk::TextureDoubleBuffered& txtr : renderTextures) { txtr = errorTexture; }

    // fill descriptor set
    const std::size_t len = textures.size() + renderTextures.size();
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
        for (unsigned int i = 0; i < renderTextures.size(); ++i, ++infoIndex) {
            auto& info       = imageInfos[infoIndex];
            info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            info.imageView   = renderTextures[i].getImages().getRaw(fi).view;
            info.sampler     = renderTextures[i].getSampler();
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
    descriptorSets->visit(visitor);
    fi = 0;
    rtDescriptorSets->visit(visitor);
    vkUpdateDescriptorSets(vulkanState.device, setWrites.size(), setWrites.data(), 0, nullptr);
}

void BindlessTextureArray::cleanup() {
    for (vk::Texture& txtr : textures) {
        if (txtr.view != errorTexture.view) { txtr.cleanup(); }
    }
    for (vk::TextureDoubleBuffered& txtr : renderTextures) {
        if (txtr.getImages().getRaw(0).view != errorTexture.view) { txtr.cleanup(); }
    }
    errorTexture.cleanup();
}

VkDescriptorSetLayoutBinding BindlessTextureArray::getLayoutBinding() const {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorCount    = textures.size() + renderTextures.size();
    binding.binding            = bindIndex;
    binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.pImmutableSamplers = 0;
    return binding;
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
    queuedUpdates.visit([texture](auto& vec) { vec.emplace_back(texture); });
}

void BindlessTextureArray::resetTexture(std::uint32_t i) {
    if (i < firstRtId) {
        vk::Texture* texture = &textures[i];
        texture->cleanup();
        *texture = errorTexture;
        texture->reset();
        updateTexture(texture);
    }
    else {
        vk::TextureDoubleBuffered* texture = &renderTextures[i - firstRtId];
        texture->cleanup();
        *texture = errorTexture;
        updateTexture(texture);
    }
}

void BindlessTextureArray::commitDescriptorUpdates() {
    if (!queuedUpdates.current().empty()) {
        // prepare descriptor updates before waiting
        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorImageInfo> infos;
        writes.resize(queuedUpdates.current().size() * 2, {});
        infos.resize(queuedUpdates.current().size() * 2, {});

        for (unsigned int j = 0; j < queuedUpdates.current().size(); ++j) {
            vk::TextureBase* texture = queuedUpdates.current()[j];
            const bool isRT          = dynamic_cast<vk::TextureDoubleBuffered*>(texture) != nullptr;
            const unsigned int k     = queuedUpdates.current().size() + j;

            std::uint32_t i;
            VkImageView view;
            if (!isRT) {
                vk::Texture* cast = static_cast<vk::Texture*>(texture);
                i                 = cast - textures.data();
                view              = cast->view;
            }
            else {
                vk::TextureDoubleBuffered* cast = static_cast<vk::TextureDoubleBuffered*>(texture);
                i                               = cast - renderTextures.data() + textures.size();
                view                            = cast->getImages().current().view;
            }

            infos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            infos[j].imageView   = view;
            infos[j].sampler     = texture->getSampler();

            // write error texture to rt set if is rt itself
            infos[k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            infos[k].imageView   = isRT ? errorTexture.view : view;
            infos[k].sampler     = isRT ? errorTexture.getSampler() : texture->getSampler();

            writes[j].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[j].descriptorCount = 1;
            writes[j].dstBinding      = bindIndex;
            writes[j].dstArrayElement = i;
            writes[j].dstSet          = descriptorSets->current();
            writes[j].pImageInfo      = &infos[j];
            writes[j].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            writes[k].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[k].descriptorCount = 1;
            writes[k].dstBinding      = bindIndex;
            writes[k].dstArrayElement = i;
            writes[k].dstSet          = rtDescriptorSets->current();
            writes[k].pImageInfo      = &infos[k];
            writes[k].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }

        queuedUpdates.current().clear();
        vkUpdateDescriptorSets(vulkanState.device, writes.size(), writes.data(), 0, nullptr);
    }
}

} // namespace res
} // namespace rc
} // namespace bl
