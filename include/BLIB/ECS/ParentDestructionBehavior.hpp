#ifndef BLIB_ECS_DESTROYBEHAVIOR_HPP
#define BLIB_ECS_DESTROYBEHAVIOR_HPP

#include <cstdint>

namespace bl
{
namespace ecs
{
/**
 * @brief Defines the behavior of an entity when any of its parents are destroyed
 *
 * @ingroup ECS
 */
enum struct ParentDestructionBehavior : std::uint8_t {
    /// Default behavior. The entity is erased with its parent
    DestroyedWithParent,

    /// The entity becomes parent-less instead of being destroyed
    OrphanedByParent
};

} // namespace ecs
} // namespace bl

#endif
