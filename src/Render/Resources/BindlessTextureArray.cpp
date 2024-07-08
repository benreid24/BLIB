#include <BLIB/Render/Resources/BindlessTextureArray.hpp>

namespace bl
{
namespace rc
{
namespace res
{
BindlessTextureArray::BindlessTextureArray(vk::VulkanState& vs, std::uint32_t size,
                                           std::uint32_t bind)
: bindIndex(bind)
, firstRtId(size - MaxRenderTextures)
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

    // fill descriptor set
    std::vector<VkDescriptorImageInfo> imageInfos(textures.size(), VkDescriptorImageInfo{});
    for (unsigned int i = 0; i < textures.size(); ++i) {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView   = textures[i].view;
        imageInfos[i].sampler     = textures[i].sampler;
    }

    std::array<VkWriteDescriptorSet, 2 * Config::MaxConcurrentFrames> setWrites{};
    unsigned int i     = 0;
    const auto visitor = [this, &i, &setWrites, &imageInfos](auto& set) {
        setWrites[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrites[i].descriptorCount = textures.size();
        setWrites[i].dstBinding      = bindIndex;
        setWrites[i].dstArrayElement = 0;
        setWrites[i].dstSet          = set;
        setWrites[i].pImageInfo      = imageInfos.data();
        setWrites[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        ++i;
    };
    descriptorSets->visit(visitor);
    rtDescriptorSets->visit(visitor);
    vkUpdateDescriptorSets(vulkanState.device, setWrites.size(), setWrites.data(), 0, nullptr);
}

void BindlessTextureArray::cleanup() {
    for (vk::Texture& txtr : textures) {
        if (txtr.view != errorTexture.view) { txtr.cleanup(); }
    }
    errorTexture.cleanup();
}

VkDescriptorSetLayoutBinding BindlessTextureArray::getLayoutBinding() const {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorCount    = textures.size();
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

void BindlessTextureArray::updateTexture(vk::Texture* texture) {
    queuedUpdates.visit([texture](auto& vec) { vec.emplace_back(texture); });
}

void BindlessTextureArray::resetTexture(std::uint32_t i) {
    vk::Texture* texture = &textures[i];
    texture->cleanup();
    *texture = errorTexture;
    updateTexture(texture);
    texture->reset();
}

void BindlessTextureArray::commitDescriptorUpdates() {
    if (!queuedUpdates.current().empty()) {
        // prepare descriptor updates before waiting
        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorImageInfo> infos;
        writes.resize(queuedUpdates.current().size() * 2, {});
        infos.resize(queuedUpdates.current().size() * 2, {});

        for (unsigned int j = 0; j < queuedUpdates.current().size(); ++j) {
            vk::Texture* texture  = queuedUpdates.current()[j];
            const std::uint32_t i = texture - textures.data();
            const bool isRT       = i >= firstRtId;
            const unsigned int k  = queuedUpdates.current().size() + j;

            infos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            infos[j].imageView   = texture->view;
            infos[j].sampler     = texture->sampler;

            // write error texture to rt set if is rt itself
            const vk::Texture& txtr = isRT ? errorTexture : textures[i];
            infos[k].imageLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            infos[k].imageView      = txtr.view;
            infos[k].sampler        = txtr.sampler;

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
