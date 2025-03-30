#ifndef BLIB_RENDER_VULKAN_TEXTUREBASE_HPP
#define BLIB_RENDER_VULKAN_TEXTUREBASE_HPP

#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Vulkan.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace res
{
class TexturePool;
class BindlessTextureArray;
} // namespace res

namespace vk
{
/**
 * @brief Base class for textures. Provides common members and boilerplate
 *
 * @ingroup Renderer
 */
class TextureBase : public tfr::Transferable {
public:
    /**
     * @brief Initializes to sane defaults
     */
    TextureBase();

    /**
     * @brief Destroys the texture
     */
    virtual ~TextureBase() = default;

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
    virtual void resize(const glm::u32vec2& size) = 0;

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
     * @brief Updates the sampler that this texture uses
     *
     * @param sampler The new sampler to use
     */
    void setSampler(VkSampler sampler);

    /**
     * @brief Returns the sampler used for this texture
     */
    VkSampler getSampler() const;

    /**
     * @brief Returns the size of the texture in pixels
     */
    const glm::u32vec2& rawSize() const;

    /**
     * @brief Returns the size of the texture in pixels
     */
    const glm::vec2& size() const;

    /**
     * @brief Returns whether or not this texture contains significant transparency (more than 10%)
     */
    bool containsTransparency() const;

    /**
     * @brief Returns the VkImage for the current frame
     */
    virtual VkImage getCurrentImage() const = 0;

    /**
     * @brief Returns the current image layout of the image for the current frame
     */
    virtual VkImageLayout getCurrentImageLayout() const = 0;

    /**
     * @brief Returns the color format of the texture
     */
    VkFormat getFormat() const;

protected:
    /**
     * @brief Derived classes should call this whenever the size changes
     *
     * @param newSize The new size of the textures
     */
    void updateSize(const glm::u32vec2& newSize);

    /**
     * @brief Derived classes should call this whenever the Vulkan handles change
     */
    void updateDescriptors();

    /**
     * @brief Derived classes should create the texture here
     *
     * @param size The size of the texture to create
     */
    virtual void create(const glm::u32vec2& size) = 0;

    /**
     * @brief Derived classes should call this when the contents change
     *
     * @param isTrans Whether the texture is significantly transparent or not
     */
    void updateTransparency(bool isTrans);

private:
    res::BindlessTextureArray* parent;
    VkSampler sampler;
    VkFormat format;
    glm::u32vec2 sizeRaw;
    glm::vec2 sizeF;
    bool hasTransparency;

    friend class res::BindlessTextureArray;
    friend class res::TexturePool;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkFormat TextureBase::getFormat() const { return format; }

inline VkSampler TextureBase::getSampler() const { return sampler; }

inline const glm::u32vec2& TextureBase::rawSize() const { return sizeRaw; }

inline const glm::vec2& TextureBase::size() const { return sizeF; }

inline bool TextureBase::containsTransparency() const { return hasTransparency; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
