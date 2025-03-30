#ifndef BLIB_RENDER_RENDERER_TEXTURE_HPP
#define BLIB_RENDER_RENDERER_TEXTURE_HPP

#include <BLIB/Render/Vulkan/TextureBase.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics/Image.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Helper struct to manage a texture in Vulkan. Textures are managed and owned by the
 *        TexturePool and should not be allocated directly
 *
 * @ingroup Renderer
 */
class Texture : public TextureBase {
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
     * @brief Resizes the texture to the given size
     *
     * @param size The new texture size
     */
    virtual void resize(const glm::u32vec2& size) override;

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
     * @brief Returns the Vulkan image handle
     */
    VkImage getImage() const;

    /**
     * @brief Returns the Vulkan image view handle
     */
    VkImageView getView() const;

private:
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
    VkImageLayout currentLayout;

    virtual void create(const glm::u32vec2& size) override;
    void createFromContentsAndQueue();
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& transferEngine) override;
    void cleanup();
    void reset();
    void updateTrans(const sf::Image& data);
    virtual VkImage getCurrentImage() const override;
    virtual VkImageLayout getCurrentImageLayout() const override;

    friend class res::TexturePool;
    friend class res::BindlessTextureArray;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkImage Texture::getImage() const { return image; }

inline VkImageView Texture::getView() const { return view; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
