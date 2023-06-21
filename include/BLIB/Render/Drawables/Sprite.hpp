#ifndef BLIB_RENDER_DRAWABLES_SPRITE_HPP
#define BLIB_RENDER_DRAWABLES_SPRITE_HPP

#include <BLIB/Render/Components/Sprite.hpp>
#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>
#include <BLIB/Render/Drawables/Components/Textured.hpp>
#include <BLIB/Render/Drawables/Components/Transform2D.hpp>
#include <BLIB/Render/Drawables/Drawable.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gfx
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
, public base::OverlayScalable
, public base::Textured {
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
};

} // namespace draw
} // namespace gfx
} // namespace bl

#endif
