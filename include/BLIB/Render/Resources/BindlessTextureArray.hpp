#ifndef BLIB_RENDER_RESOURCES_BINDLESSTEXTUREARRAY_HPP
#define BLIB_RENDER_RESOURCES_BINDLESSTEXTUREARRAY_HPP

#include <BLIB/Render/Resources/Texture.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <vector>

namespace bl
{
namespace gfx
{
/// Collection of classes for renderer resource management
namespace res
{
/**
 * @brief Utility class to help manage an array of Sampler2D as part of a descriptor set. Utilized
 *        by TexturePool and MaterialPool. The expectation is that the user manages lifetime, index
 *        assignment, etc. This class just provides some boilerplate.
 *
 * @ingroup Renderer
 */
class BindlessTextureArray {
public:
    /**
     * @brief Creates a new bindless texture array manager
     *
     * @param vulkanState The renderer Vulkan state
     * @param arraySize The size of the array descriptor item
     * @param bindIndex The bind index for shaders in the descriptor set
     */
    BindlessTextureArray(vk::VulkanState& vulkanState, std::uint32_t arraySize,
                         std::uint32_t bindIndex);

    /**
     * @brief Returns a layout binding to be used for descriptor set layout creation
     */
    VkDescriptorSetLayoutBinding getLayoutBinding() const;

    /**
     * @brief Initializes the array and writes to the descriptor set. Fill the error pattern if a
     *        custom error texture is desired, otherwise it is filled with a bright purple
     *
     * @param descriptorSet The descriptor set to write to
     */
    void init(VkDescriptorSet descriptorSet);

    /**
     * @brief Destroys the array and frees all textures
     */
    void cleanup();

    /**
     * @brief Fetches the given texture from the array
     *
     * @param i The index of the texture to fetch
     * @return The texture at the given index
     */
    Texture& getTexture(std::uint32_t i);

    /**
     * @brief Loads the texture contents for the given texture from the given resource id. Utilizes
     *        ResourceManager<sf::Image> to load the contents. Falls back on the error content if
     *        loading fails. Does not perform update. Call commitTexture() to update.
     *
     * @param i The index of the texture to load
     * @param path The resource id of the content to load into the texture
     */
    void prepareTextureUpdate(std::uint32_t i, const std::string& path);

    /**
     * @brief Returns the bind index of this array within its descriptor set
     */
    constexpr std::uint32_t getBindIndex() const;

    /**
     * @brief Returns the image to use as the error pattern. Fill this with custom content if
     *        desired. init() defaults it to bright purple if not already filled
     */
    constexpr sf::Image& getErrorPattern();

    /**
     * @brief Helper method to send descriptor writes for the given texture across a batch of
     *        texture arrays. Useful for uses like MaterialPool where each id has several associated
     *        textures. This method sends over updated texture contents from prepareTextureUpdate()
     *
     * @tparam N The number of arrays to update the given index across
     * @param descriptorSet The descriptor set to update
     * @param arrays A set of texture arrays to use to update
     * @param i The id of the texture across the arrays to update
     */
    template<std::size_t N>
    static void commitTexture(VkDescriptorSet descriptorSet,
                              const std::array<BindlessTextureArray*, N>& arrays, std::uint32_t i);

    /**
     * @brief Updates the descriptor set for the given texture
     *
     * @param texture The texture to update descriptors for
     */
    void updateTexture(Texture* texture);

    /**
     * @brief Helper method to send descriptor writes for the given texture across a batch of
     *        texture arrays. Useful for uses like MaterialPool where each id has several associated
     *        textures. This method resets the texture to the error image for each array
     *
     * @tparam N The number of arrays to update the given index across
     * @param descriptorSet The descriptor set to update
     * @param arrays A set of texture arrays to use to update
     * @param i The id of the texture across the arrays to reset
     */
    template<std::size_t N>
    static void resetTexture(VkDescriptorSet descriptorSet,
                             const std::array<BindlessTextureArray*, N>& arrays, std::uint32_t i);

private:
    const std::uint32_t bindIndex;
    vk::VulkanState& vulkanState;
    sf::Image errorPattern;
    Texture errorTexture;
    std::vector<Texture> textures;
    VkDescriptorSet descriptorSet;

    friend class Texture;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr std::uint32_t BindlessTextureArray::getBindIndex() const { return bindIndex; }

inline constexpr sf::Image& BindlessTextureArray::getErrorPattern() { return errorPattern; }

inline Texture& BindlessTextureArray::getTexture(std::uint32_t i) { return textures[i]; }

template<std::size_t N>
void BindlessTextureArray::commitTexture(VkDescriptorSet descriptorSet,
                                         const std::array<BindlessTextureArray*, N>& arrays,
                                         std::uint32_t i) {
    vk::VulkanState& vulkanState = arrays[0]->vulkanState;

    for (BindlessTextureArray* array : arrays) { array->textures[i].createFromContentsAndQueue(); }

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
void BindlessTextureArray::resetTexture(VkDescriptorSet descriptorSet,
                                        const std::array<BindlessTextureArray*, N>& arrays,
                                        std::uint32_t i) {
    for (BindlessTextureArray* array : arrays) {
        array->textures[i].cleanup();
        array->textures[i] = array->errorTexture;
    }
    commitTexture(descriptorSet, arrays, i);
}

} // namespace res
} // namespace gfx
} // namespace bl

#endif
