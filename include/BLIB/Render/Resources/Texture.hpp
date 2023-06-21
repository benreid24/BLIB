#ifndef BLIB_RENDER_RENDERER_TEXTURE_HPP
#define BLIB_RENDER_RENDERER_TEXTURE_HPP

#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics/Image.hpp>
#include <glad/vulkan.h>
#include <glm/glm.hpp>
#include <vk_mem_alloc.h>

namespace bl
{
namespace gfx
{
namespace vk
{
struct VulkanState;
}
namespace res
{
class TexturePool;
class BindlessTextureArray;

/**
 * @brief Helper struct to manage a texture in Vulkan. Textures are managed and owned by the
 *        TexturePool and should not be allocated directly
 *
 * @ingroup Renderer
 */
class Texture : public tfr::Transferable {
public:
    /**
     * @brief Creates an empty Texture
     *
     */
    Texture();

    /**
     * @brief Does nothing
     */
    virtual ~Texture() = default;

    /**
     * @brief Resizes the texture to be the given size unless it is already bigger
     *
     * @param size The size to ensure
     */
    void ensureSize(const glm::u32vec2& size);

    /**
     * @brief Updates the content of the texture from the given image. Performs no bounds validation
     *
     * @param content The image containing the content to copy
     * @param destPos Offset into the texture to copy the new content
     * @param source Region from the source to copy
     */
    void update(const sf::Image& content, const glm::u32vec2& destPos = {0, 0},
                const sf::IntRect& source = {});

    /**
     * @brief Updates the content of the texture from the given image. Performs no bounds validation
     *
     * @param content The image containing the content to copy
     * @param destPos Offset into the texture to copy the new content
     * @param source Region from the source to copy
     */
    void update(const resource::Ref<sf::Image>& content, const glm::u32vec2& destPos = {0, 0},
                const sf::IntRect& source = {});

    /**
     * @brief Updates the sampler that this texture uses
     *
     * @param sampler The new sampler to use
     */
    void setSampler(VkSampler sampler);

    /**
     * @brief Returns the size of the texture in pixels
     */
    constexpr const glm::u32vec2& rawSize() const;

    /**
     * @brief Returns the size of the texture in pixels
     */
    constexpr const glm::vec2& size() const;

private:
    BindlessTextureArray* parent;

    // transfer data
    const sf::Image* altImg;
    resource::Ref<sf::Image> transferImg;
    glm::u32vec2 destPos;
    sf::IntRect source;

    // texture data
    VkImage image;
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
    VkImageView view;
    VkSampler sampler;
    glm::u32vec2 sizeRaw;
    glm::vec2 sizeF;

    // resize data
    bool needsCleanup;
    VkImage oldImage;
    VmaAllocation oldAlloc;
    VkImageView oldView;

    void create(const glm::u32vec2& size);
    void createFromContentsAndQueue();
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& transferEngine) override;
    void cleanup();
    void queueCleanup();

    friend class TexturePool;
    friend class BindlessTextureArray;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::u32vec2& Texture::rawSize() const { return sizeRaw; }

inline constexpr const glm::vec2& Texture::size() const { return sizeF; }

} // namespace res
} // namespace gfx
} // namespace bl

#endif
