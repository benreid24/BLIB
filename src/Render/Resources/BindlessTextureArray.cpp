#include <BLIB/Render/Resources/BindlessTextureArray.hpp>

namespace bl
{
namespace render
{
BindlessTextureArray::BindlessTextureArray(VulkanState& vs, std::uint32_t size, std::uint32_t bind)
: bindIndex(bind)
, vulkanState(vs)
, textures(size) {}

void BindlessTextureArray::init(VkDescriptorSet descriptorSet) {
    // ensure an error pattern exists
    if (errorPattern.getSize().x == 0) { errorPattern.create(32, 32, sf::Color(245, 66, 242)); }

    // init error pattern texture
    errorTexture.externalContents = &errorPattern;
    errorTexture.createFromContentsAndQueue(vulkanState);
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
        if (txtr.view != errorTexture.view) { txtr.cleanup(vulkanState); }
    }
    errorTexture.cleanup(vulkanState);
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

} // namespace render
} // namespace bl
