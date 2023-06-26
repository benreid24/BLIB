#include <BLIB/Render/Resources/BindlessTextureArray.hpp>

namespace bl
{
namespace gfx
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
}

void BindlessTextureArray::init(VkDescriptorSet ds, VkDescriptorSet rtds) {
    descriptorSet   = ds;
    rtDescriptorSet = rtds;

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

    VkWriteDescriptorSet setWrites[2]{};
    setWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[0].descriptorCount = textures.size();
    setWrites[0].dstBinding      = bindIndex;
    setWrites[0].dstArrayElement = 0;
    setWrites[0].dstSet          = descriptorSet;
    setWrites[0].pImageInfo      = imageInfos.data();
    setWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    setWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[1].descriptorCount = textures.size();
    setWrites[1].dstBinding      = bindIndex;
    setWrites[1].dstArrayElement = 0;
    setWrites[1].dstSet          = rtDescriptorSet;
    setWrites[1].pImageInfo      = imageInfos.data();
    setWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkUpdateDescriptorSets(vulkanState.device, 2, setWrites, 0, nullptr);
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

void BindlessTextureArray::updateTexture(vk::Texture* texture) {
    const std::uint32_t i = texture - textures.data();
    const bool isRT       = i >= firstRtId;

    VkDescriptorImageInfo imageInfos[2]{};
    imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfos[0].imageView   = texture->view;
    imageInfos[0].sampler     = texture->sampler;

    // write error texture to rt set if is rt itself
    const vk::Texture& txtr   = isRT ? errorTexture : textures[i];
    imageInfos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfos[1].imageView   = txtr.view;
    imageInfos[1].sampler     = txtr.sampler;

    VkWriteDescriptorSet setWrites[2]{};
    setWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[0].descriptorCount = 1;
    setWrites[0].dstBinding      = bindIndex;
    setWrites[0].dstArrayElement = i;
    setWrites[0].dstSet          = descriptorSet;
    setWrites[0].pImageInfo      = &imageInfos[0];
    setWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    setWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[1].descriptorCount = 1;
    setWrites[1].dstBinding      = bindIndex;
    setWrites[1].dstArrayElement = i;
    setWrites[1].dstSet          = rtDescriptorSet;
    setWrites[1].pImageInfo      = &imageInfos[1];
    setWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    vkUpdateDescriptorSets(vulkanState.device, 2, setWrites, 0, nullptr);
}

} // namespace res
} // namespace gfx
} // namespace bl
