#ifndef BLIB_ENTITIES_EVENTS_HPP
#define BLIB_ENTITIES_EVENTS_HPP

#include <BLIB/Entities/Entity.hpp>

namespace bl
{
namespace entity
{
/// Collection of events pertaining to entities
namespace event
{
/**
 * @brief Fired after an entity is added to the Registry
 *
 * @ingroup Entities
 *
 */
struct EntityCreated {
    /// The entity that was just created
    const Entity entity;
};

/**
 * @brief Fired right before an entity is removed from the registry
 *
 * @ingroup Entities
 *
 */
struct EntityDestroyed {
    /// The entity that is about to get destroyed
    const Entity entity;
};

} // namespace event
} // namespace entity
} // namespace bl

#endif
