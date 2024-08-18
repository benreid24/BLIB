#ifndef BLIB_RENDER_VULKAN_TEXTUREDOUBLEBUFFERED_HPP
#define BLIB_RENDER_VULKAN_TEXTUREDOUBLEBUFFERED_HPP

#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/TextureBase.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
class Texture;

/**
 * @brief Double buffered texture that provides the backing for RenderTexture
 *
 * @ingroup Renderer
 */
class TextureDoubleBuffered : public TextureBase {
public:
    /**
     * @brief Data for each image in a double buffered texture
     */
    struct Image {
        VkImage image;
        VmaAllocation alloc;
        VmaAllocationInfo allocInfo;
        VkImageView view;
    };

    /**
     * @brief Initializes to sane defaults
     */
    TextureDoubleBuffered();

    /**
     * @brief Destroys the textures
     */
    virtual ~TextureDoubleBuffered() = default;

    /**
     * @brief Resizes the texture to the given size
     *
     * @param size The new texture size
     */
    virtual void resize(const glm::u32vec2& size) override;

    /**
     * @brief Access the images for this texture
     */
    const PerFrame<Image>& getImages() const;

private:
    PerFrame<Image> images;

    virtual void create(const glm::u32vec2& size) override;
    TextureDoubleBuffered& operator=(const Texture& txtr);
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& transferEngine) override;
    void cleanup();
    virtual VkImage getCurrentImage() const override;
    virtual VkImageLayout getCurrentImageLayout() const override;
    virtual VkFormat getFormat() const override;

    friend class res::BindlessTextureArray;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const PerFrame<TextureDoubleBuffered::Image>& TextureDoubleBuffered::getImages() const {
    return images;
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
