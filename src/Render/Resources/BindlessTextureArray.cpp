#include <BLIB/Render/Resources/BindlessTextureArray.hpp>

namespace bl
{
namespace render
{
namespace res
{
BindlessTextureArray::BindlessTextureArray(vk::VulkanState& vs, std::uint32_t size,
                                           std::uint32_t bind)
: bindIndex(bind)
, vulkanState(vs)
, textures(size) {
    errorTexture.vulkanState = &vs;
    errorTexture.parent      = this;
    for (auto& t : textures) {
        t.parent      = this;
        t.vulkanState = &vs;
    }
}

void BindlessTextureArray::init(VkDescriptorSet ds) {
    descriptorSet = ds;

    // ensure an error pattern exists
    if (errorPattern.getSize().x == 0) { errorPattern.create(32, 32, sf::Color(245, 66, 242)); }

    // init error pattern texture
    errorTexture.altImg = &errorPattern;
    errorTexture.createFromContentsAndQueue();
    errorTexture.sampler = vulkanState.samplerCache.filteredRepeated();
    vulkanState.transferEngine.executeTransfers();

    // init all textures to error pattern
    for (Texture& txtr : textures) { txtr = errorTexture; }

    // fill descriptor set
    std::vector<VkDescriptorImageInfo> imageInfos(textures.size(), VkDescriptorImageInfo{});
    for (unsigned int i = 0; i < textures.size(); ++i) {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView   = textures[i].view;
        imageInfos[i].sampler     = textures[i].sampler;
    }

    VkWriteDescriptorSet setWrite{};
    setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrite.descriptorCount = textures.size();
    setWrite.dstBinding      = bindIndex;
    setWrite.dstArrayElement = 0;
    setWrite.dstSet          = descriptorSet;
    setWrite.pImageInfo      = imageInfos.data();
    setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkUpdateDescriptorSets(vulkanState.device, 1, &setWrite, 0, nullptr);
}

void BindlessTextureArray::cleanup() {
    for (Texture& txtr : textures) {
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

void BindlessTextureArray::updateTexture(Texture* texture) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = texture->view;
    imageInfo.sampler     = texture->sampler;

    VkWriteDescriptorSet setWrite{};
    setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrite.descriptorCount = 1;
    setWrite.dstBinding      = bindIndex;
    setWrite.dstArrayElement = texture - textures.data();
    setWrite.dstSet          = descriptorSet;
    setWrite.pImageInfo      = &imageInfo;
    setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkUpdateDescriptorSets(vulkanState.device, 1, &setWrite, 0, nullptr);
}

} // namespace res
} // namespace render
} // namespace bl
