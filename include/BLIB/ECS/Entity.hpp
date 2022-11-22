#ifndef BLIB_ECS_ENTITY_HPP
#define BLIB_ECS_ENTITY_HPP

#include <cstdint>
#include <limits>
#include <mutex>

namespace bl
{
/// Contains the Entity Component System
namespace ecs
{
/**
 * @brief Entity is simply an id. Components are added via a Registry and they own all the
 *        relevant properties. Systems can query lists of Entities with certain components and
 *        perform logic and updates
 *
 * @ingroup Entity
 *
 */
using Entity = std::uint32_t;

/**
 * @brief Special value to indicate that an Entity is not valid
 *
 * @ingroup ECS
 *
 */
constexpr Entity InvalidEntity = std::numeric_limits<Entity>::max();

} // namespace ecs

} // namespace bl

#endif
