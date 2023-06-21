#ifndef BLIB_RENDER_EVENTS_OVERLAYENTITYSCALED_HPP
#define BLIB_RENDER_EVENTS_OVERLAYENTITYSCALED_HPP

#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace gfx
{
namespace event
{
/**
 * @brief Fired when OverlayScaler updates an entity
 *
 * @ingroup Renderer
 */
struct OverlayEntityScaled {
    /**
     * @brief Creates a new event
     *
     * @param entity The entity which got scaled
     */
    OverlayEntityScaled(ecs::Entity entity)
    : entity(entity) {}

    const ecs::Entity entity;
};

} // namespace event
} // namespace gfx
} // namespace bl

#endif
