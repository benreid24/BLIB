#ifndef BLIB_GRAPHICS_SPRITE_HPP
#define BLIB_GRAPHICS_SPRITE_HPP

#include <BLIB/Components/Sprite.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Components/Textured.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gfx
{
/**
 * @brief SFML-like sprite class to render images in scenes or overlays. Wraps the ECS interface
 *
 * @ingroup Renderer
 */
class Sprite
: public Drawable<com::Sprite>
, public bcom::OverlayScalable
, public bcom::Textured {
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
    Sprite(engine::Engine& engine, rc::res::TextureRef texture, const sf::FloatRect& region = {});

    /**
     * @brief Creates the ECS backing for the sprite
     *
     * @param engine The game engine instance
     * @param texture The texture for the sprite
     * @param region The region to render from the texture
     */
    void create(engine::Engine& engine, rc::res::TextureRef texture,
                const sf::FloatRect& region = {});
};

} // namespace gfx
} // namespace bl

#endif
