#ifndef BLIB_RENDER_RESOURCES_TEXTUREREF_HPP
#define BLIB_RENDER_RESOURCES_TEXTUREREF_HPP

#include <cstdint>

namespace bl
{
namespace gfx
{
namespace res
{
class TexturePool;
class Texture;

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
    constexpr const Texture& operator*() const;

    /**
     * @brief Access the underlying texture
     */
    constexpr Texture& operator*();

    /**
     * @brief Access the underlying texture
     */
    constexpr const Texture* operator->() const;

    /**
     * @brief Access the underlying texture
     */
    constexpr Texture* operator->();

    /**
     * @brief Access the underlying texture
     */
    constexpr Texture* get();

    /**
     * @brief Access the underlying texture
     */
    constexpr const Texture* get() const;

private:
    TexturePool* owner;
    Texture* texture;

    TextureRef(TexturePool& owner, Texture& texture);

    void addRef();

    friend class TexturePool;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const Texture& TextureRef::operator*() const { return *texture; }

inline constexpr const Texture* TextureRef::operator->() const { return texture; }

inline constexpr Texture& TextureRef::operator*() { return *texture; }

inline constexpr Texture* TextureRef::operator->() { return texture; }

inline constexpr Texture* TextureRef::get() { return texture; }

inline constexpr const Texture* TextureRef::get() const { return texture; }

} // namespace res
} // namespace gfx
} // namespace bl

#endif
