#ifndef BLIB_COMPONENTS_ANIMATION2D_HPP
#define BLIB_COMPONENTS_ANIMATION2D_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
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
struct Animation2D : public rc::rcom::DescriptorComponentBase<Animation2D, std::uint32_t> {
    ecs::Entity playerEntity;
    std::uint32_t playerIndex; // assigned by SlideshowInstance

    /**
     * @brief Creates an empty animation component
     */
    Animation2D()
    : playerEntity(ecs::InvalidEntity)
    , playerIndex(0) {}

    /**
     * @brief Creates an animation component using the given player
     *
     * @param player ECS entity that has the player component to use
     */
    Animation2D(ecs::Entity player)
    : playerEntity(player)
    , playerIndex(0) {}
    /**
     * @brief Sets the animation player to use
     *
     * @param entity The ECS entity with the Animation2DPlayer to use
     */
    void setPlayer(ecs::Entity entity) {
        playerEntity = entity;
        markDirty();
    }

    /**
     * @brief Updates the player index in the renderer descriptor buffer
     *
     * @param payload The index to update
     */
    void refreshDescriptor(std::uint32_t& payload) { payload = playerIndex; }
};

} // namespace com
} // namespace bl

#endif
