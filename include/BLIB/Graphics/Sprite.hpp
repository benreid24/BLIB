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
 * @ingroup Graphics
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
     * @brief Creates the ECS backing for the sprite on an existing entity
     *
     * @param engine The game engine instance
     * @param existingEntity The entity to add the sprite components to
     * @param texture The texture for the sprite
     * @param region The region to render from the texture
     */
    Sprite(engine::Engine& engine, ecs::Entity existingEntity, rc::res::TextureRef texture,
           const sf::FloatRect& region = {});

    /**
     * @brief Creates the ECS backing for the sprite
     *
     * @param engine The game engine instance
     * @param texture The texture for the sprite
     * @param region The region to render from the texture
     */
    void create(engine::Engine& engine, rc::res::TextureRef texture,
                const sf::FloatRect& region = {});

    /**
     * @brief Creates the ECS backing for the sprite on an existing entity
     *
     * @param engine The game engine instance
     * @param existingEntity The entity to add the sprite components to
     * @param texture The texture for the sprite
     * @param region The region to render from the texture
     */
    void create(engine::Engine& engine, ecs::Entity existingEntity, rc::res::TextureRef texture,
                const sf::FloatRect& region = {});

    /**
     * @brief Sets the texture for this sprite. Use on already created sprites
     *
     * @param texture The texture to use
     */
    void setTexture(rc::res::TextureRef texture);

    /**
     * @brief Sets the texture for this sprite. Use on already created sprites
     *
     * @param texture The texture to use
     * @param region The region to render from the texture
     */
    void setTexture(rc::res::TextureRef texture, const sf::FloatRect region);

    /**
     * @brief Convenience method that calls setColor on the underlying component
     *
     * @param color The color to multiply the texture by
     */
    void setColor(const sf::Color& color);

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

protected:
    /**
     * @brief Called when the local size is queried
     */
    virtual void ensureLocalSizeUpdated() override;
};

} // namespace gfx
} // namespace bl

#endif
