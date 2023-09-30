#ifndef BLIB_ECS_ENTITY_HPP
#define BLIB_ECS_ENTITY_HPP

#include <cstdint>
#include <functional>
#include <limits>
#include <mutex>
#include <ostream>

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
 * @ingroup ECS
 *
 */
struct Entity {
    using IdType = std::uint64_t;

    /**
     * @brief Special value to indicate that an Entity is not valid
     */
    static constexpr IdType InvalidId          = std::numeric_limits<IdType>::max();
    static constexpr std::uint64_t IndexMask   = 0x00000000FFFFFFFF;
    static constexpr std::uint64_t VersionMask = 0xFFFF000000000000;

    IdType id;

    /**
     * @brief Initializes an invalid entity
     */
    constexpr Entity()
    : id(InvalidId) {}

    /**
     * @brief Initializes the entity with the given composite id
     *
     * @param id The composite id to use
     */
    Entity(IdType id)
    : id(id) {}

    /**
     * @brief Composes the entity from its components
     *
     * @param index The 0-based index of the entity
     * @param version The version of the index
     */
    Entity(std::uint64_t index, std::uint64_t version)
    : id((version << 48) | index) {}

    /**
     * @brief Copies the given entity
     *
     * @param entity The entity to copy
     */
    Entity(const Entity& entity)
    : id(entity.id) {}

    /**
     * @brief Copies the given entity
     *
     * @param entity The entity to copy
     */
    Entity(Entity&& entity)
    : id(entity.id) {}

    /**
     * @brief Copies the given entity
     *
     * @param entity The entity to copy
     * @return A reference to this entity
     */
    Entity& operator=(const Entity& right) {
        id = right.id;
        return *this;
    }

    /**
     * @brief Copies the given entity
     *
     * @param entity The entity to copy
     * @return A reference to this entity
     */
    Entity& operator=(Entity&& right) {
        id = right.id;
        return *this;
    }

    /**
     * @brief Returns the 0-based index for the entity
     *
     * @return The 0-based index of the entity
     */
    std::uint64_t getIndex() const { return id & IndexMask; }

    /**
     * @brief Returns the version of an entity. The same entity index may be reused, the version
     *        differentiates it from the prior instances
     *
     * @param entity The entity to get the version for
     * @return The version number of the entity
     */
    std::uint64_t getVersion() const { return (id & VersionMask) >> 48; }

    /**
     * @brief Helper implicit conversion operator to the underlying id type
     */
    operator IdType() const { return id; }

    /**
     * @brief Tests the equality against the given entity
     *
     * @param right The entity to compare with
     * @return True if equal, false otherwise
     */
    bool operator==(const Entity& right) const { return id == right.id; }

    /**
     * @brief Tests the equality against the given entity
     *
     * @param right The entity to compare with
     * @return True if not equal, false otherwise
     */
    bool operator!=(const Entity& right) const { return id != right.id; }

    /**
     * @brief Outputs the entity in a human friendly format
     *
     * @param os The stream to output to
     * @param entity The entity to output
     * @return Returns os
     */
    friend std::ostream& operator<<(std::ostream& os, Entity entity) {
        os << entity.getIndex() << "-v" << entity.getVersion();
        return os;
    }
};

/**
 * @brief Special value to indicate that an Entity is not valid
 */
static constexpr Entity InvalidEntity = Entity();

} // namespace ecs
} // namespace bl

namespace std
{
template<>
struct hash<bl::ecs::Entity> {
    std::size_t operator()(bl::ecs::Entity ent) const {
        return std::hash<bl::ecs::Entity::IdType>()(ent.id);
    }
};
} // namespace std

#endif
