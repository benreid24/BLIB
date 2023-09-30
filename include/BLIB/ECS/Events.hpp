#ifndef BLIB_ECS_EVENTS_HPP
#define BLIB_ECS_EVENTS_HPP

#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace ecs
{
template<typename T>
class ComponentPool;

/// Collection of events pertaining to entities
namespace event
{
/**
 * @brief Fired after an entity is added to the Registry
 *
 * @ingroup ECS
 *
 */
struct EntityCreated {
    /// The entity that was just created
    const Entity entity;
};

/**
 * @brief Fired right before an entity is removed from the registry
 *
 * @ingroup ECS
 *
 */
struct EntityDestroyed {
    /// The entity that is about to get destroyed
    const Entity entity;
};

/**
 * @brief Fired when a component is added to an entity
 *
 * @tparam T The type of component that was added
 * @ingroup ECS
 */
template<typename T>
struct ComponentAdded {
    /// The entity that got the component
    const Entity entity;

    /// The component that was added
    const T& component;

    /**
     * @brief Construct a new ComponentAdded event
     *
     * @param e The entity the component belongs to
     * @param component The component that was added
     */
    ComponentAdded(Entity e, const T& component)
    : entity(e)
    , component(component) {}
};

/**
 * @brief Fired right before a component is removed from an entity
 *
 * @tparam T The type of component that is about to be removed
 * @ingroup ECS
 */
template<typename T>
struct ComponentRemoved {
    /// The entity the component belongs to
    const Entity entity;

    /// The component that is about to be removed
    const T& component;

    /**
     * @brief Construct a new ComponentRemoved event
     *
     * @param e The entity the component belongs to
     * @param component The component that was removed
     */
    ComponentRemoved(Entity e, const T& component)
    : entity(e)
    , component(component) {}
};

/**
 * @brief Fired when a component pool is resized
 *
 * @ingroup ECS
 */
struct ComponentPoolResized {
    /// The pool that resized
    std::uint16_t poolIndex;

    /**
     * @brief Creates a new ComponentPoolResized event
     * @param pool The pool that resized
     */
    ComponentPoolResized(std::uint16_t pool)
    : poolIndex(pool) {}
};

/**
 * @brief Fired when an entity has a parent set
 *
 * @ingroup ECS
 */
struct EntityParentSet {
    /// The entity's new parent
    Entity parent;

    /// The entity that had the parent set
    Entity child;

    /**
     * @brief Creates a new parent event
     *
     * @param parent The parent entity id
     * @param child The child entity that had the parent set
     */
    EntityParentSet(Entity parent, Entity child)
    : parent(parent)
    , child(child) {}
};

/**
 * @brief Fired when an entity's parent is removed
 *
 * @ingroup ECS
 */
struct EntityParentRemoved {
    /// The entity whose parent was removed
    Entity orphan;

    /**
     * @brief Creates a new parent event
     *
     * @param orphan The entity whose parent was removed
     */
    EntityParentRemoved(Entity orphan)
    : orphan(orphan) {}
};

/**
 * @brief Fired when an entity is marked as a dependency of another
 *
 * @ingroup ECS
 */
struct EntityDependencyAdded {
    /// The entity being depended on
    Entity resource;

    /// The entity depending on resource
    Entity user;

    /**
     * @brief Creates a new resource event
     *
     * @param resource The entity being depended on
     * @param user The entity depending on resource
     */
    EntityDependencyAdded(Entity resource, Entity user)
    : resource(resource)
    , user(user) {}
};

/**
 * @brief Fired when an entity no longer depends on another
 *
 * @ingroup ECS
 */
struct EntityDependencyRemoved {
    /// The entity no longer being depended on
    Entity resource;

    /// The entity no longer depending on resource
    Entity user;

    /**
     * @brief Creates a new resource event
     *
     * @param resource The entity no longer being depended on
     * @param user The entity no longer depending on resource
     */
    EntityDependencyRemoved(Entity resource, Entity user)
    : resource(resource)
    , user(user) {}
};

} // namespace event
} // namespace ecs
} // namespace bl

#endif
