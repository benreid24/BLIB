#ifndef BLIB_RENDER_COMPONENTS_SPRITE_HPP
#define BLIB_RENDER_COMPONENTS_SPRITE_HPP

#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Primitives/IndexBuffer.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace render
{
class Renderer;

namespace com
{
/**
 * @brief Sprite component repersenting a 2d image for an entity
 *
 * @ingroup Renderer
 */
class Sprite : public DrawableBase {
public:
    /**
     * @brief Creates an empty sprite
     */
    Sprite() = default;

    /**
     * @brief Creates the sprite so that it may be added to a scene
     *
     * @param renderer The renderer instance
     * @param texture The texture to render
     * @param region The region of the texture to render
     */
    Sprite(Renderer& renderer, const res::TextureRef& texture, const sf::FloatRect& region = {});

    /**
     * @brief Creates the sprite so that it may be added to a scene
     *
     * @param renderer The renderer instance
     * @param texture The texture to render
     * @param region The region of the texture to render
     */
    void create(Renderer& renderer, const res::TextureRef& texture, sf::FloatRect region = {});

    /**
     * @brief Swap out the texture to use. Does not reset texture region. Call create() to reset
     *        region texture
     *
     * @param texture The new texture to render
     */
    void setTexture(const res::TextureRef& texture);

    /**
     * @brief Returns the texture that is rendered by the sprite
     */
    constexpr const res::TextureRef& getTexture() const;

    /**
     * @brief Returns the pre-transform size of the Sprite
     */
    constexpr const glm::vec2& getSize() const;

private:
    prim::IndexBuffer buffer;
    res::TextureRef texture;
    glm::vec2 size;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const res::TextureRef& Sprite::getTexture() const { return texture; }

inline constexpr const glm::vec2& Sprite::getSize() const { return size; }

} // namespace com
} // namespace render
} // namespace bl

#endif
