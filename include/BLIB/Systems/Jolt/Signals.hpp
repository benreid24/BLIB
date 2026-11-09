#ifndef BLIB_SYSTEMS_JOLT_SIGNALS_HPP
#define BLIB_SYSTEMS_JOLT_SIGNALS_HPP

#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace sys
{
namespace jolt
{
/**
 * @brief Fired when a collision happens between two entities
 *
 * @ingroup Systems
 */
struct CollisionStartedEvent {
    ecs::Entity entityA;
    ecs::Entity entityB;
};

/**
 * @brief Fired when a collision persists frame over frame between two entities
 *
 * @ingroup Systems
 */
struct CollisionPersistedEvent {
    ecs::Entity entityA;
    ecs::Entity entityB;
};

/**
 * @brief Fired when two entities cease to be colliding
 *
 * @ingroup Systems
 */
struct CollisionEndedEvent {
    ecs::Entity entityA;
    ecs::Entity entityB;
};

} // namespace jolt
} // namespace sys
} // namespace bl

#endif
