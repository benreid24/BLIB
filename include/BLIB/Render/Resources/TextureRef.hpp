#ifndef BLIB_RENDER_RESOURCES_TEXTUREREF_HPP
#define BLIB_RENDER_RESOURCES_TEXTUREREF_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace vk
{
class Texture;
class TextureBase;
class TextureDoubleBuffered;
} // namespace vk

namespace res
{
class TexturePool;

/**
 * @brief Lightweight reference to a texture inside of a TexturePool. Once all references to a
 *        texture are destructed or released then the texture may be freed
 *
 * @ingroup Renderer
 */
class TextureRef {
public:
    /**
     * @brief Create an empty TextureRef
     */
    TextureRef();

    /**
     * @brief Copies from the given reference. Increments the reference count
     *
     * @param copy The reference to copy from
     */
    TextureRef(const TextureRef& copy);

    /**
     * @brief Copies from the given reference without incrementing the reference count. Invalidates
     *        the source reference
     *
     * @param move The reference to copy from and invalidate
     */
    TextureRef(TextureRef&& move);

    /**
     * @brief Releases the reference if not already invalidated
     */
    ~TextureRef();

    /**
     * @brief Copies from the given reference. Increments the reference count
     *
     * @param copy The reference to copy from
     * @return A reference to this object
     */
    TextureRef& operator=(const TextureRef& copy);

    /**
     * @brief Copies from the given reference without incrementing the reference count. Invalidates
     *        the source reference
     *
     * @param move The reference to copy from and invalidate
     * @return A reference to this object
     */
    TextureRef& operator=(TextureRef&& move);

    /**
     * @brief Returns whether the ref points to a texture or not
     */
    operator bool() const;

    /**
     * @brief Releases this reference to the underlying texture
     */
    void release();

    /**
     * @brief Returns the internal id of this texture
     */
    std::uint32_t id() const;

    /**
     * @brief Access the underlying texture
     */
    const vk::TextureBase& operator*() const;

    /**
     * @brief Access the underlying texture
     */
    vk::TextureBase& operator*();

    /**
     * @brief Access the underlying texture
     */
    const vk::TextureBase* operator->() const;

    /**
     * @brief Access the underlying texture
     */
    vk::TextureBase* operator->();

    /**
     * @brief Access the underlying texture
     */
    vk::TextureBase* get();

    /**
     * @brief Access the underlying texture
     */
    const vk::TextureBase* get() const;

    /**
     * @brief Access the underlying texture as a bindless texture
     */
    vk::Texture* asBindlessTexture();

    /**
     * @brief Access the underlying texture as a bindless texture
     */
    const vk::Texture* asBindlessTexture() const;

    /**
     * @brief Access the underlying texture as a render texture
     */
    vk::TextureDoubleBuffered* asRenderTexture();

    /**
     * @brief Access the underlying texture as a render texture
     */
    const vk::TextureDoubleBuffered* asRenderTexture() const;

private:
    TexturePool* owner;
    vk::TextureBase* texture;
    std::uint32_t arrayId;

    TextureRef(TexturePool& owner, vk::TextureBase& texture, std::uint32_t arrayId);

    void addRef();
    static void disableCleanup();

    friend class TexturePool;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline std::uint32_t TextureRef::id() const { return arrayId; }

inline const vk::TextureBase& TextureRef::operator*() const { return *texture; }

inline const vk::TextureBase* TextureRef::operator->() const { return texture; }

inline vk::TextureBase& TextureRef::operator*() { return *texture; }

inline vk::TextureBase* TextureRef::operator->() { return texture; }

inline vk::TextureBase* TextureRef::get() { return texture; }

inline const vk::TextureBase* TextureRef::get() const { return texture; }

} // namespace res
} // namespace rc
} // namespace bl

#endif
