#ifndef BLIB_RENDER_RENDERER_TEXTURE_HPP
#define BLIB_RENDER_RENDERER_TEXTURE_HPP

#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Image.hpp>
#include <BLIB/Render/Vulkan/Sampler.hpp>
#include <BLIB/Render/Vulkan/SamplerOptions.hpp>
#include <BLIB/Render/Vulkan/TextureOptions.hpp>
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
class Texture : public tfr::Transferable {
public:
    /**
     * @brief The type of texture that this is
     */
    enum struct Type {
        Texture2D = ImageOptions::Type::Image2D,
        Cubemap   = ImageOptions::Type::Cubemap,
        RenderTexture
    };

    /**
     * @brief Creates an empty Texture
     */
    Texture();

    /**
     * @brief Does nothing
     */
    virtual ~Texture() = default;

    /**
     * @brief Returns the type of texture this is
     */
    Type getType() const;

    /**
     * @brief Returns the normalized coordinate for this texture, taking into account atlasing
     *        performed by the renderer
     *
     * @param src The texture coordinate to convert
     * @return The texture coordinate to use for geometry
     */
    glm::vec2 convertCoord(const glm::vec2& src) const;

    /**
     * @brief Performs convertCoord but first normalizes the coordinate to this texture
     *
     * @param src The unnormalized coordinate to normalize and convert
     * @return The coordinate to use for geometry
     */
    glm::vec2 normalizeAndConvertCoord(const glm::vec2& src) const;

    /**
     * @brief Resizes the texture to be the given size unless it is already bigger
     *
     * @param size The size to ensure
     */
    void ensureSize(const glm::u32vec2& size);

    /**
     * @brief Resizes the texture to the given size
     *
     * @param size The new texture size
     */
    void resize(const glm::u32vec2& size);

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
    void setSampler(SamplerOptions::Type sampler);

    /**
     * @brief Returns the sampler used for this texture
     */
    SamplerOptions::Type getSampler() const;

    /**
     * @brief Returns the Vulkan image handle
     */
    VkImage getImage() const;

    /**
     * @brief Returns the Vulkan image view handle
     */
    VkImageView getView() const;

    /**
     * @brief Returns the color format of the texture
     */
    VkFormat getFormat() const;

    /**
     * @brief Returns whether or not this texture contains significant transparency (more than 10%)
     */
    bool containsTransparency() const;

    /**
     * @brief Returns the size of the texture in pixels
     */
    glm::u32vec2 rawSize() const;

    /**
     * @brief Returns the size of the texture in pixels
     */
    glm::vec2 size() const;

    /**
     * @brief Returns the current image layout of the image for the current frame
     */
    VkImageLayout getCurrentImageLayout() const;

private:
    // base data
    res::TexturePool* parent;
    Type type;
    TextureOptions createOptions;
    Image image;
    VkImageView currentView;
    bool hasTransparency;

    // transfer data
    const sf::Image* altImg;
    resource::Ref<sf::Image> transferImg;
    sf::Image localImage;
    glm::u32vec2 destPos;
    sf::IntRect source;

    void create(Type type, const glm::u32vec2& size, const TextureOptions& options);
    void createFromContentsAndQueue(Type type, const TextureOptions& options);
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& transferEngine) override;
    void cleanup();
    void reset();
    void updateTrans(const sf::Image& data);
    void updateDescriptors();

    friend class res::TexturePool;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkImage Texture::getImage() const { return image.getImage(); }

inline VkImageView Texture::getView() const { return image.getView(); }

inline VkFormat Texture::getFormat() const { return image.getFormat(); }

inline glm::u32vec2 Texture::rawSize() const {
    return {image.getSize().width, image.getSize().height};
}

inline glm::vec2 Texture::size() const { return glm::vec2(rawSize()); }

inline bool Texture::containsTransparency() const { return hasTransparency; }

inline SamplerOptions::Type Texture::getSampler() const { return createOptions.sampler; }

inline VkImageLayout Texture::getCurrentImageLayout() const { return image.getCurrentLayout(); }

inline Texture::Type Texture::getType() const { return type; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
