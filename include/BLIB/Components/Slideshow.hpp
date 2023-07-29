#ifndef BLIB_COMPONENTS_SLIDESHOW_HPP
#define BLIB_COMPONENTS_SLIDESHOW_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/StableHandle.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <cstdint>

namespace bl
{
namespace com
{
/**
 * @brief Component that indicates that an entity is a Slideshow animation. Simply references the
 *        Animation2DPlayer player the entity should use
 *
 * @ingroup Components
 */
struct Slideshow : public rc::rcom::DescriptorComponentBase<Slideshow, std::uint32_t> {
    ecs::Entity playerEntity;
    std::uint32_t playerIndex; // assigned by SlideshowInstance

    /**
     * @brief Sets the animation player to use
     *
     * @param entity The ECS entity with the Animation2DPlayer to use
     */
    void setPlayer(ecs::Entity entity) {
        playerEntity = entity;
        // TODO - how to update player index on change?
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
