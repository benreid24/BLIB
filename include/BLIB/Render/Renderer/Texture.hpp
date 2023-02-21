#ifndef BLIB_RENDER_RENDERER_TEXTURE_HPP
#define BLIB_RENDER_RENDERER_TEXTURE_HPP

#include <BLIB/Render/Vulkan/Transferable.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics/Image.hpp>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

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
struct Texture : public Transferable {
    /**
     * @brief Creates an empty Texture
     *
     */
    Texture();

    /**
     * @brief Does nothing
     */
    virtual ~Texture() = default;

    VkImage image;
    VkDeviceMemory memory;
    std::uint32_t memoryOffset;
    VkDeviceSize memorySize;
    VkImageView view;
    VkSampler sampler;
    glm::u32vec2 size;
    glm::vec2 sizeF;

private:
    const sf::Image* altImg;
    resource::Ref<sf::Image> transferImg;

    void createFromContentsAndQueue(VulkanState& vs);
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 TransferEngine& transferEngine) override;
    void cleanup(VulkanState& vs);

    friend class TexturePool;
    friend class BindlessTextureArray;
};

} // namespace render
} // namespace bl

#endif
