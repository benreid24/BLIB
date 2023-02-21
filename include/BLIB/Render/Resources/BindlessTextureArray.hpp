#ifndef BLIB_RENDER_RESOURCES_BINDLESSTEXTUREARRAY_HPP
#define BLIB_RENDER_RESOURCES_BINDLESSTEXTUREARRAY_HPP

#include <BLIB/Render/Renderer/Texture.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <vector>

namespace bl
{
namespace render
{
class BindlessTextureArray {
public:
    BindlessTextureArray(VulkanState& vulkanState, std::uint32_t arraySize,
                         std::uint32_t bindIndex);

    VkDescriptorSetLayoutBinding getLayoutBinding() const;

    void init(VkDescriptorSet descriptorSet);

    void cleanup();

    Texture& getTexture(std::uint32_t i);

    constexpr std::uint32_t getBindIndex() const;

    constexpr sf::Image& getErrorPattern();

    template<std::size_t N>
    static void commitTexture(VulkanState& vulkanState, VkDescriptorSet descriptorSet,
                              const std::array<BindlessTextureArray*, N>& arrays, std::uint32_t i);

    template<std::size_t N>
    static void resetTexture(VulkanState& vulkanState, VkDescriptorSet descriptorSet,
                             const std::array<BindlessTextureArray*, N>& arrays, std::uint32_t i);

private:
    const std::uint32_t bindIndex;
    VulkanState& vulkanState;
    sf::Image errorPattern;
    Texture errorTexture;
    std::vector<Texture> textures;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr std::uint32_t BindlessTextureArray::getBindIndex() const { return bindIndex; }

inline constexpr sf::Image& BindlessTextureArray::getErrorPattern() { return errorPattern; }

inline Texture& BindlessTextureArray::getTexture(std::uint32_t i) { return textures[i]; }

template<std::size_t N>
void BindlessTextureArray::commitTexture(VulkanState& vulkanState, VkDescriptorSet descriptorSet,
                                         const std::array<BindlessTextureArray*, N>& arrays,
                                         std::uint32_t i) {
    for (BindlessTextureArray* array : arrays) {
        array->textures[i].createFromContentsAndQueue(vulkanState);
    }

    VkDescriptorImageInfo imageInfos[N]{};
    for (std::size_t j = 0; j < N; ++j) {
        imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[j].imageView   = arrays[j]->textures[i].view;
        imageInfos[j].sampler     = arrays[j]->textures[i].sampler;
    }

    VkWriteDescriptorSet setWrites[N]{};
    for (std::size_t j = 0; j < N; ++j) {
        setWrites[j].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrites[j].descriptorCount = 1;
        setWrites[j].dstBinding      = arrays[j]->bindIndex;
        setWrites[j].dstArrayElement = i;
        setWrites[j].dstSet          = descriptorSet;
        setWrites[j].pImageInfo      = &imageInfos[j];
        setWrites[j].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
    vkUpdateDescriptorSets(vulkanState.device, N, setWrites, 0, nullptr);
}

template<std::size_t N>
void BindlessTextureArray::resetTexture(VulkanState& vulkanState, VkDescriptorSet descriptorSet,
                                        const std::array<BindlessTextureArray*, N>& arrays,
                                        std::uint32_t i) {
    for (BindlessTextureArray* array : arrays) {
        array->textures[i].cleanup(vulkanState);
        array->textures[i] = array->errorTexture;
    }
    commitTexture(vulkanState, descriptorSet, arrays, i);
}

} // namespace render
} // namespace bl

#endif
