#ifndef BLIB_ECS_REGISTRY_HPP
#define BLIB_ECS_REGISTRY_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/View.hpp>
#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <cstdlib>
#include <memory>
#include <utility>

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
     * @brief Creates a new entity registry with space for entityCount entities
     *
     * @param entityCount The maximum number of entities to store
     */
    Registry(std::size_t entityCount);

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
     * @param entity The entity to add it to
     * @param value The initial component value
     * @return T* Pointer to the new component
     */
    template<typename T>
    T* addComponent(Entity entity, const T& value);

    /**
     * @brief Adds a new component of the given type to the given entity
     *
     * @tparam T The type of component to add
     * @param entity The entity to add it to
     * @param value The initial component value
     * @return T* Pointer to the new component
     */
    template<typename T>
    T* addComponent(Entity entity, T&& value);

    /**
     * @brief Constructs the given component in-place for the given entity
     *
     * @tparam T The type of component to add
     * @tparam TArgs The constructor parameter types
     * @param entity The entity to add it to
     * @param args The arguments to the component's constructor
     * @return T* Pointer to the new component
     */
    template<typename T, typename... TArgs>
    T* emplaceComponent(Entity entity, TArgs&&... args);

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

    // entity id management
    std::mutex entityLock;
    util::IdAllocator<Entity> entityAllocator;
    std::vector<ComponentMask::Value> entityMasks;

    // components
    std::vector<ComponentPoolBase*> componentPools;
    std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> poolMap;

    // views
    std::vector<std::unique_ptr<ViewBase>> views;

    template<typename T>
    ComponentPool<T>& getPool();

    template<typename... TComponents>
    void populateView(View<TComponents...>& view);
    template<typename... TComponents>
    void populateViewWithLock(View<TComponents...>& view);

    template<typename T>
    void finishComponentAdd(Entity ent, unsigned int cindex, T* component);

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
    std::lock_guard lock(entityLock);

    auto& pool = getPool<T>();
    T* nc      = pool.add(ent, val);
    finishComponentAdd<T>(ent, pool.ComponentIndex, nc);
    return nc;
}

template<typename T>
T* Registry::addComponent(Entity ent, T&& val) {
    std::lock_guard lock(entityLock);

    auto& pool = getPool<T>();
    T* nc      = pool.add(ent, val);
    finishComponentAdd<T>(ent, pool.ComponentIndex, nc);
    return nc;
}

template<typename T, typename... TArgs>
T* Registry::emplaceComponent(Entity ent, TArgs&&... args) {
    std::lock_guard lock(entityLock);

    auto& pool = getPool<T>();
    T* nc      = pool.emplace(ent, std::forward<TArgs>(args)...);
    finishComponentAdd<T>(ent, pool.ComponentIndex, nc);
    return nc;
}

template<typename T>
void Registry::finishComponentAdd(Entity ent, unsigned int cIndex, T* component) {
    bl::event::Dispatcher::dispatch<event::ComponentAdded<T>>({ent, *component});
    ComponentMask::Value& mask = entityMasks[ent];
    ComponentMask::add(mask, cIndex);
    for (auto& view : views) {
        if (ComponentMask::completelyContains(mask, view->mask)) { view->tryAddEntity(ent); }
    }
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
    std::lock_guard lock(entityLock);

    auto& pool = getPool<T>();
    pool.remove(ent);
    ComponentMask::Value& mask = entityMasks[ent];
    for (auto& view : views) {
        if (ComponentMask::completelyContains(mask, view->mask)) { view->removeEntity(ent); }
    }
    ComponentMask::remove(mask, pool.ComponentIndex);
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
    const std::array<std::uint8_t, sizeof...(TComponents)> indices(
        {static_cast<std::uint8_t>(getPool<TComponents>().ComponentIndex)...});
    ComponentMask::Value mask = ComponentMask::EmptyMask;
    for (const std::uint8_t i : indices) { ComponentMask::add(mask, i); }

    // find existing view
    for (auto& view : views) {
        if (view->mask == mask) return static_cast<View<TComponents...>*>(view.get());
    }

    // create new view
    views.emplace_back(new View<TComponents...>(*this, maxEntities, mask));
    return static_cast<View<TComponents...>*>(views.back().get());
}

template<typename T>
ComponentPool<T>& Registry::getPool() {
    const std::type_index tid = typeid(T);
    auto it                   = poolMap.find(tid);
    if (it == poolMap.end()) {
#ifdef BLIB_DEBUG
        if (poolMap.size() >= ComponentMask::MaxComponentTypeCount) {
            BL_LOG_CRITICAL << "Maximum component types reached with component: " << tid.name()
                            << ". Specify BLIB_ECS_USE_WIDE_MASK to increase max allowed.";
            std::exit(1);
        }
#endif
        it = poolMap.try_emplace(tid, new ComponentPool<T>(poolMap.size(), maxEntities)).first;
        componentPools.emplace_back(it->second.get());
    }

    return *static_cast<ComponentPool<T>*>(componentPools[it->second->ComponentIndex]);
}

template<typename... TComponents>
void Registry::populateView(View<TComponents...>& view) {
    for (Entity ent = 0; ent <= entityAllocator.highestId(); ++ent) {
        if (entityMasks[ent] == view.mask) { view.tryAddEntity(ent); }
    }
}

template<typename... TComponents>
void Registry::populateViewWithLock(View<TComponents...>& view) {
    std::lock_guard lock(entityLock);
    populateView(view);
}

} // namespace ecs
} // namespace bl

#endif
