#ifndef BLIB_ECS_REGISTRY_HPP
#define BLIB_ECS_REGISTRY_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/View.hpp>
#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <cstdlib>
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
    std::mutex entityLock;
    std::vector<bool> aliveEntities;
    std::vector<ComponentMask::Value> entityMasks;
    container::RingQueue<Entity> freeEntities;
    Entity nextEntity;

    // components
    std::vector<ComponentPoolBase*> componentPools;

    // views
    std::vector<std::unique_ptr<ViewBase>> views;

    Registry(std::size_t entityCount, bl::event::Dispatcher& eventBus);

    template<typename T>
    ComponentPool<T>& getPool();

    template<typename... TComponents>
    void populateView(View<TComponents...>& view);

    template<typename... TComponents>
    friend class View;
    friend class engine::Engine;
};

} // namespace ecs
} // namespace bl

#include <BLIB/ECS/ComponentSetImpl.hpp>
#include <BLIB/ECS/ViewImpl.hpp>

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

namespace bl
{
namespace ecs
{
template<typename T>
T* Registry::addComponent(Entity ent, const T& val) {
    auto& pool = getPool<T>();
    T* nc      = pool.add(ent, val);
    eventBus.dispatch<event::ComponentAdded<T>>({ent, *nc});
    ComponentMask::add(entityMasks[ent], pool.ComponentIndex);
    for (auto& view : views) {
        if (ComponentMask::has(view->mask, pool.ComponentIndex)) { view->tryAddEntity(*this, ent); }
    }
    return nc;
}

template<typename T>
T* Registry::getComponent(Entity ent) {
    return getPool<T>().get(ent);
}

template<typename T>
bool Registry::hasComponent(Entity ent) {
    return getPool<T>().get(ent) != nullptr;
}

template<typename T>
void Registry::removeComponent(Entity ent) {
    auto& pool = getPool<T>();
    pool.remove(ent, eventBus);
    ComponentMask::remove(entityMasks[ent], pool.ComponentIndex);
    for (auto& view : views) {
        if (ComponentMask::has(view->mask, pool.ComponentIndex)) { view->removeEntity(ent); }
    }
}

template<typename T>
ComponentPool<T>& Registry::getAllComponents() {
    return getPool<T>();
}

template<typename... TComponents>
ComponentSet<TComponents...> Registry::getComponentSet(Entity ent) {
    return ComponentSet<TComponents...>(*this, ent);
}

template<typename... TComponents>
View<TComponents...>* Registry::getOrCreateView() {
    // calculate component mask
    const std::array<std::uint8_t, sizeof(TComponents)...> indices(
        {getPool<TComponents>().ComponentIndex...});
    ComponentMask::Value mask = ComponentMask::EmptyMask;
    for (const std::uint8_t i : indices) { ComponentMask::add(mask, i); }

    // find existing view
    for (auto& view : views) {
        if (view->mask == mask) return dynamic_cast<View<TComponents...>*>(view.get());
    }

    // create new view
    views.emplace_back(new View<TComponents...>(*this, maxEntities, mask));
    return dynamic_cast<View<TComponents...>*>(views.back().get());
}

template<typename T>
ComponentPool<T>& Registry::getPool() {
    ComponentPool<T>& pool = ComponentPool<T>::get(maxEntities);
    if (pool.ComponentIndex == componentPools.size()) {
        if (pool.ComponentIndex >= ComponentPoolBase::MaximumComponentCount) {
            BL_LOG_CRITICAL << "Maximum component type count reached on component: "
                            << typeid(T).name();
            std::exit(1);
        }
        componentPools.emplace_back(&pool);
    }
    return pool;
}

template<typename... TComponents>
void Registry::populateView(View<TComponents...>& view) {
    std::lock_guard lock(entityLock);

    for (Entity ent = 0; ent < nextEntity; ++ent) {
        if (entityMasks[ent] == view.mask) { view.tryAddEntity(*this, ent); }
    }
}

} // namespace ecs
} // namespace bl

#endif
