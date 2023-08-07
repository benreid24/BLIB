#ifndef BLIB_COMPONENTS_ANIMATION2D_HPP
#define BLIB_COMPONENTS_ANIMATION2D_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <cstdint>

namespace bl
{
namespace com
{
/**
 * @brief Component that indicates that an entity is a 2d Animation. Simply references the
 *        Animation2DPlayer player the entity should use
 *
 * @ingroup Components
 */
// TODO - slideshow/animation split here?
struct Animation2D : public rc::rcom::DrawableBase {
    ecs::Entity playerEntity;
    // TODO - index buffer of slideshow vertex

    /**
     * @brief Creates an empty animation component
     */
    Animation2D()
    : playerEntity(ecs::InvalidEntity) {}

    /**
     * @brief Creates an animation component using the given player
     *
     * @param player ECS entity that has the player component to use
     */
    Animation2D(ecs::Entity player)
    : playerEntity(player) {}

    /**
     * @brief Sets the animation player to use
     *
     * @param entity The ECS entity with the Animation2DPlayer to use
     */
    void setPlayer(ecs::Entity entity) {
        playerEntity = entity;
        // TODO
    }
};

} // namespace com
} // namespace bl

#endif
