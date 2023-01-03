#ifndef BLIB_RENDER_COMPONENTS_TEXTURE_HPP
#define BLIB_RENDER_COMPONENTS_TEXTURE_HPP

#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>
#include <glad/vulkan.h>
#include <string>

namespace bl
{
namespace render
{
struct VulkanState;
class TexturePool;

/**
 * @brief Helper struct to manage a texture in Vulkan. Textures are managed and owned by the
 *        TexturePool and should not be allocated directly
 *
 * @ingroup Renderer
 */
class Texture {
public:
    /**
     * @brief Destroy the Texture object
     *
     */
    ~Texture();

    /**
     * @brief Creates (or recreates) the sampler for shaders to use with this texture
     *
     * @param addressMode How to handle sampling out-of-bounds coordinates
     * @param magFilter The filter to use when magnifying
     * @param minFilter The filter to use when minifying
     */
    void createSampler(VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                       VkFilter magFilter               = VK_FILTER_LINEAR,
                       VkFilter minFilter               = VK_FILTER_LINEAR);

    /**
     * @brief Creates (or recreates) the sampler for shaders to use with this texture
     *
     * @param addressModeU How to handle sampling out-of-bounds X coordinates
     * @param addressModeV How to handle sampling out-of-bounds Y coordinates
     * @param addressModeW How to handle sampling out-of-bounds Z coordinates
     * @param magFilter The filter to use when magnifying
     * @param minFilter The filter to use when minifying
     */
    void createSampler(VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV,
                       VkSamplerAddressMode addressModeW, VkFilter magFilter = VK_FILTER_LINEAR,
                       VkFilter minFilter = VK_FILTER_LINEAR);

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

private:
    Texture(VulkanState& rendererState);

    void destroy(VkDevice device);
    bool create(const std::string& imagePath);
    bool create(const sf::Image& image);
};

} // namespace render
} // namespace bl

#endif
