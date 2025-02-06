#ifndef BLIB_RENDER_RESOURCES_BINDLESSTEXTUREARRAY_HPP
#define BLIB_RENDER_RESOURCES_BINDLESSTEXTUREARRAY_HPP

#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>
#include <BLIB/Render/Vulkan/TextureDoubleBuffered.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <vector>

namespace bl
{
namespace rc
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
     * @brief Initializes the array and writes to the descriptor set. Fill the error pattern if a
     *        custom error texture is desired, otherwise it is filled with a bright purple
     *
     * @param descriptorSets The descriptors set to write to
     * @param rtDescriptorSets Descriptor sets to write to for render texture rendering
     */
    void init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
              vk::PerFrame<VkDescriptorSet>& rtDescriptorSets);

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
    vk::TextureBase& getTexture(std::uint32_t i);

    /**
     * @brief Loads the texture contents for the given texture from the given resource id. Utilizes
     *        ResourceManager<sf::Image> to load the contents. Falls back on the error content if
     *        loading fails. Does not perform update. Call updateTexture() to update.
     *
     * @param i The index of the texture to load
     * @param path The resource id of the content to load into the texture
     */
    void prepareTextureUpdate(std::uint32_t i, const std::string& path);

    /**
     * @brief Prepares the texture to populate with the given source image
     *
     * @param i The index of the texture to update
     * @param src The source image to populate with
     */
    void prepareTextureUpdate(std::uint32_t i, const sf::Image& src);

    /**
     * @brief Returns the bind index of this array within its descriptor set
     */
    std::uint32_t getBindIndex() const;

    /**
     * @brief Returns the image to use as the error pattern. Fill this with custom content if
     *        desired. init() defaults it to bright purple if not already filled
     */
    sf::Image& getErrorPattern();

    /**
     * @brief Updates the descriptor set for the given texture. Updates are performed in sync with
     *        the GPU to avoid modifying the descriptor set while in use
     *
     * @param texture The texture to update descriptors for
     */
    void updateTexture(vk::TextureBase* texture);

    /**
     * @brief Resets the given texture to the default error texture
     *
     * @param i The index of the texture to reset
     */
    void resetTexture(std::uint32_t i);

    /**
     * @brief Performs the descriptor updates for all queued textures. Waits for the device to be
     *        idle only if there are queued textures
     *
     * @param setWriter The descriptor set updater to use
     * @param currentSet The current standard texture descriptor set
     * @param currentRtSet The current render texture descriptor set
     */
    void commitDescriptorUpdates(ds::SetWriteHelper& setWriter, VkDescriptorSet currentSet,
                                 VkDescriptorSet currentRtSet);

private:
    const std::uint32_t bindIndex;
    const std::uint32_t firstRtId;
    vk::VulkanState& vulkanState;
    sf::Image errorPattern;
    vk::Texture errorTexture;
    std::vector<vk::Texture> textures;
    std::vector<vk::TextureDoubleBuffered> renderTextures;
    vk::PerFrame<std::vector<vk::TextureBase*>> queuedUpdates;

    friend class vk::Texture;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline std::uint32_t BindlessTextureArray::getBindIndex() const { return bindIndex; }

inline sf::Image& BindlessTextureArray::getErrorPattern() { return errorPattern; }

inline vk::TextureBase& BindlessTextureArray::getTexture(std::uint32_t i) {
    return i >= firstRtId ? static_cast<vk::TextureBase&>(renderTextures[i - firstRtId]) :
                            static_cast<vk::TextureBase&>(textures[i]);
}

} // namespace res
} // namespace rc
} // namespace bl

#endif
