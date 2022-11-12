#ifndef BLIB_ECS_REGISTRY_HPP
#define BLIB_ECS_REGISTRY_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/View.hpp>
#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <memory>

namespace bl
{
namespace engine
{
class Engine;
}

namespace ecs
{
/**
 * @brief This is the main interface of the ECS. This class manages all entities and components and
 *        views and serves as the primary interface to be used.
 *
 * @ingroup ECS
 *
 */
class Registry : private util::NonCopyable {
public:
    /**
     * @brief Creates a new entity and returns its id. May fail if full
     *
     * @return Entity The new entity id, or InvalidEntity if full
     */
    Entity createEntity();

    /**
     * @brief Returns whether or not the given entity exists
     *
     * @param entity The entity to test for
     * @return True if the entity is in the ECS, false otherwise
     */
    bool entityExists(Entity entity) const;

    /**
     * @brief Removes the given entity and destroys all of its components
     *
     * @param entity The entity to remove from the ECS
     */
    void destroyEntity(Entity entity);

    /**
     * @brief Destroys and removes all entities and components from the ECS
     *
     */
    void destroyAllEntities();

    /**
     * @brief Adds a new component of the given type to the given entity
     *
     * @tparam T The type of component to add
     * @param entity Thje entity to add it to
     * @param value The initial component value
     * @return T* Pointer to the new component
     */
    template<typename T>
    T* addComponent(Entity entity, const T& value);

    /**
     * @brief Retrieves the given component belonging to the given entity
     *
     * @tparam T The type of component to get
     * @param entity The entity to get the component for
     * @return T* Pointer to the entity's component or nullptr if not present
     */
    template<typename T>
    T* getComponent(Entity entity);

    /**
     * @brief Returns whether or not the entity has the given component
     *
     * @tparam T The component type to check for
     * @param entity The entity to check on
     * @return True if the entity has the component, false otherwise
     */
    template<typename T>
    bool hasComponent(Entity entity);

    /**
     * @brief Removes the given component from the given entity
     *
     * @tparam T The type of component to remove
     * @param entity The entity to remove the component from
     */
    template<typename T>
    void removeComponent(Entity entity);

    /**
     * @brief Returns an iterable component pool for the given component type
     *
     * @tparam T The component type
     * @return ComponentPool<T>& A pool of all components of the given type that may be iterated
     */
    template<typename T>
    ComponentPool<T>& getAllComponents();

    /**
     * @brief Gets a set of components for the given entity
     *
     * @tparam TComponents The types of components to fetch
     * @param entity The entity to get the components from
     * @return ComponentSet<TComponents...> The set of fetched components. May be invalid
     */
    template<typename... TComponents>
    ComponentSet<TComponents...> getComponentSet(Entity entity);

    /**
     * @brief Fetches or creates a view that returns an iterable set of entities that contain the
     *        given components
     *
     * @tparam TComponents The components to filter on
     * @return View<TComponents...>* A pointer to a view that returns all matching entities
     */
    template<typename... TComponents>
    View<TComponents...>* getOrCreateView();

private:
    const std::size_t maxEntities;
    bl::event::Dispatcher& eventBus;

    // entity id management
    std::vector<bool> existingEntities;
    container::RingQueue<Entity> freeEntities;
    Entity nextEntity;

    // components
    std::vector<ComponentPoolBase*> componentPools;

    // views
    std::vector<std::unique_ptr<ViewBase>> views;

    Registry(std::size_t entityCount, bl::event::Dispatcher& eventBus);
    void engineUpdate();

    friend class Engine;
};

} // namespace ecs
} // namespace bl

#include <BLIB/ECS/ComponentSetImpl.hpp>

#endif
