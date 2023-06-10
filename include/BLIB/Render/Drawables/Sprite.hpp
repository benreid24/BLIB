#ifndef BLIB_RENDER_DRAWABLES_SPRITE_HPP
#define BLIB_RENDER_DRAWABLES_SPRITE_HPP

#include <BLIB/Render/Components/Sprite.hpp>
#include <BLIB/Render/Drawables/Components/Textured.hpp>
#include <BLIB/Render/Drawables/Components/Transformable.hpp>
#include <BLIB/Render/Drawables/Drawable.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace render
{
namespace draw
{
/**
 * @brief SFML-like sprite class to render images in scenes or overlays. Wraps the ECS interface
 *
 * @ingroup Renderer
 */
class Sprite
: public Drawable<com::Sprite>
, public base::Textured
, public base::Transformable {
public:
    /**
     * @brief Creates an uninitialized sprite
     */
    Sprite() = default;

    /**
     * @brief Creates the ECS backing for the sprite
     *
     * @param engine The game engine instance
     * @param texture The texture for the sprite
     * @param region The region to render from the texture
     */
    Sprite(engine::Engine& engine, res::TextureRef texture, const sf::FloatRect& region = {});

    /**
     * @brief Destroys the ECS entity of this sprite
     */
    ~Sprite();

    /**
     * @brief Creates the ECS backing for the sprite
     *
     * @param engine The game engine instance
     * @param texture The texture for the sprite
     * @param region The region to render from the texture
     */
    void create(engine::Engine& engine, res::TextureRef texture, const sf::FloatRect& region = {});

    /**
     * @brief Destroys the ECS entity of this sprite
     */
    void destroy();

    /**
     * @brief Scales the transform to take up a percentage of the given overlay width. Maintains
     *        aspect ratio
     *
     * @param ratio Percentage of overlay width to take up, in range [0, 1]
     * @param overlayWidth Width of the overlay
     */
    void scaleWidthToOverlay(float ratio, float overlayWidth = 1.f);

    /**
     * @brief Scales the transform to take up a percentage of the given overlay height. Maintains
     *        aspect ratio
     *
     * @param ratio Percentage of overlay height to take up, in range [0, 1]
     * @param overlayHeight Height of the overlay
     */
    void scaleHeightToOverlay(float ratio, float overlayHeigt = 1.f);

    /**
     * @brief Scales to take up a percentage of overlay width and height. Does not maintain AR
     *
     * @param ratios Width and height percentages in ranges [0, 1]
     * @param overlaySize Width and height of the overlay
     */
    void scaleToOverlay(const glm::vec2& ratios, const glm::vec2& overlaySize = {1.f, 1.f});
};

} // namespace draw
} // namespace render
} // namespace bl

#endif
