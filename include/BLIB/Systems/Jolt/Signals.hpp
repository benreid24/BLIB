#ifndef BLIB_SYSTEMS_JOLT_SIGNALS_HPP
#define BLIB_SYSTEMS_JOLT_SIGNALS_HPP

#include <Jolt/Jolt.h>

#include <BLIB/ECS/Entity.hpp>
#include <Jolt/Physics/Body/Body.h>

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
    const JPH::Body& entityABody;
    ecs::Entity entityB;
    const JPH::Body& entityBBody;
};

/**
 * @brief Fired when a collision persists frame over frame between two entities
 *
 * @ingroup Systems
 */
struct CollisionPersistedEvent {
    ecs::Entity entityA;
    const JPH::Body& entityABody;
    ecs::Entity entityB;
    const JPH::Body& entityBBody;
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
