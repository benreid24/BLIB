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
using Entity = std::uint64_t;

/**
 * @brief Special value to indicate that an Entity is not valid
 *
 * @ingroup ECS
 *
 */
constexpr Entity InvalidEntity = std::numeric_limits<Entity>::max();

/**
 * @brief Helpers for extracting entity metadata from entity ids
 *
 * @ingroup ECS
 */
struct IdUtil {
    static constexpr std::uint64_t IndexMask   = 0x00000000FFFFFFFF;
    static constexpr std::uint64_t VersionMask = 0xFFFF000000000000;

    /**
     * @brief Returns the 0-based index for the given entity
     *
     * @param entity The entity to get the index for
     * @return The 0-based index of the entity
     */
    static std::uint64_t getEntityIndex(Entity entity) { return entity & IndexMask; }

    /**
     * @brief Returns the version of an entity. The same entity index may be reused, the version
     *        differentiates it from the prior instances
     *
     * @param entity The entity to get the version for
     * @return The version number of the entity
     */
    static std::uint64_t getEntityVersion(Entity entity) { return (entity & VersionMask) >> 48; }

    /**
     * @brief Creates an entity id from the given index and version
     * 
     * @param index The index of the entity
     * @param version The version of the entity
     * @return The combined entity id
    */
    static Entity composeEntity(std::uint64_t index, std::uint64_t version) {
        return (version << 48) | index;
    }
};

} // namespace ecs

} // namespace bl

#endif
