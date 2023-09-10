#ifndef BLIB_ECS_REGISTRY_HPP
#define BLIB_ECS_REGISTRY_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/DependencyGraph.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/ParentGraph.hpp>
#include <BLIB/ECS/View.hpp>
#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Util/IdAllocatorUnbounded.hpp>
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
 */
class Registry
: private util::NonCopyable
, public bl::event::Listener<event::ComponentPoolResized> {
public:
    static constexpr std::size_t DefaultCapacity = 512;

    /**
     * @brief Creates a new entity registry
     */
    Registry();

    /**
     * @brief Creates a new entity and returns its id
     *
     * @return Entity The new entity id
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
     * @brief Removes the given entity and destroys all of its components. Destruction will not
     *        occur if the entity is depended on by others. In that case it will be marked for later
     *        removal once the dependencies are removed
     *
     * @param entity The entity to remove from the ECS
     * @return True if the entity was removed or was already destroyed, false if removal was blocked
     */
    bool destroyEntity(Entity entity);

    /**
     * @brief Destroys and removes all entities and components from the ECS
     *
     */
    void destroyAllEntities();

    /**
     * @brief Sets the parent entity of the given entity
     *
     * @param child The entity to set the parent of
     * @param parent The parent of the given entity
     */
    void setEntityParent(Entity child, Entity parent);

    /**
     * @brief Removes the parent of the given entity, if any
     *
     * @param child The entity to orphan
     */
    void removeEntityParent(Entity child);

    /**
     * @brief Adds a dependency on resource from user. Controls whether or not an entity may be
     *        safely deleted
     *
     * @param resource The entity being depended on
     * @param user The entity using the resource
     */
    void addDependency(Entity resource, Entity user);

    /**
     * @brief Removes the given dependency. Will destroy the resource if a prior call to
     *        destroyEntity() failed for it
     *
     * @param resource The entity being depended on
     * @param user The entity using the resource
     */
    void removeDependency(Entity resource, Entity user);

    /**
     * @brief Calls removeDependency() and then destroys the resource if nothing else depends on it.
     *        If it still has dependencies then it will be marked for removal once all dependencies
     *        are removed
     *
     * @param resource The resource being used
     * @param user The entity using the resource
     */
    void removeDependencyAndDestroyIfPossible(Entity resource, Entity user);

    /**
     * @brief Returns whether or not the given entity is depended on by other entities
     *
     * @param resource The entity to check
     * @return True if other entities depend on it, false otherwise
     */
    bool isDependedOn(Entity resource) const;

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
     * @tparam TRequire Required tagged components. ie Require<int, char>
     * @tparam TOptional Optional tagged components. ie Optional<bool>
     * @param entity The entity to get the components from
     * @return ComponentSet<TComponents...> The set of fetched components. May be invalid
     */
    template<typename TRequire, typename TOptional = Optional<>>
    ComponentSet<TRequire, TOptional> getComponentSet(Entity entity);

    /**
     * @brief Fetches or creates a view that returns an iterable set of entities that contain the
     *        given components
     *
     * @tparam TComponents The components to filter on
     * @return View<TComponents...>* A pointer to a view that returns all matching entities
     */
    template<typename TRequire, typename TOptional = Optional<>, typename TExclude = Exclude<>>
    View<TRequire, TOptional, TExclude>* getOrCreateView();

private:
    // entity id management
    mutable std::mutex entityLock;
    util::IdAllocatorUnbounded<std::uint32_t> entityAllocator;
    std::vector<ComponentMask::SimpleMask> entityMasks;
    std::vector<std::uint16_t> entityVersions;

    // entity relationships
    ParentGraph parentGraph;
    DependencyGraph dependencyGraph;
    std::vector<bool> markedForRemoval;

    // components
    std::vector<ComponentPoolBase*> componentPools;
    std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> poolMap;

    // views
    std::vector<std::unique_ptr<priv::ViewBase>> views;

    template<typename T>
    ComponentPool<T>& getPool();

    bool entityExistsLocked(Entity ent) const;
    void markEntityForRemoval(Entity ent);

    template<typename TRequire, typename TOptional, typename TExclude>
    void populateView(View<TRequire, TOptional, TExclude>& view);
    template<typename TRequire, typename TOptional, typename TExclude>
    void populateViewWithLock(View<TRequire, TOptional, TExclude>& view);

    template<typename T>
    void finishComponentAdd(Entity ent, unsigned int cindex, T* component);

    virtual void observe(const event::ComponentPoolResized& resize) override;

    template<typename TRequire, typename TOptional, typename TExclude>
    friend class View;
    friend class engine::Engine;
};

} // namespace ecs
} // namespace bl

#include <BLIB/ECS/ComponentSetImpl.hpp>
#include <BLIB/ECS/TagsImpl.hpp>
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
    ComponentMask::SimpleMask& mask        = entityMasks[IdUtil::getEntityIndex(ent)];
    const ComponentMask::SimpleMask ogMask = mask;
    ComponentMask::add(mask, cIndex);
    for (auto& view : views) {
        if (view->mask.passes(mask)) { view->tryAddEntity(ent); }
        else if (view->mask.passes(ogMask) && ComponentMask::has(view->mask.excluded, cIndex)) {
            view->removeEntity(ent);
        }
    }

    // notify pools of parent set
    auto& pool = getPool<T>();
    for (Entity child : parentGraph.getChildren(ent)) {
        const std::uint32_t ic = IdUtil::getEntityIndex(child);
        const ComponentMask mask{.required = entityMasks[ic]};
        if (mask.contains(pool.ComponentIndex)) { pool.onParentSet(child, ent); }
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

    // notify pools of parent remove on children
    for (Entity child : parentGraph.getChildren(ent)) {
        const std::uint32_t ic = IdUtil::getEntityIndex(child);
        const ComponentMask mask{.required = entityMasks[ic]};
        if (mask.contains(pool.ComponentIndex)) { pool.onParentRemove(child); }
    }

    // do remove
    pool.remove(ent);
    ComponentMask::SimpleMask& mask = entityMasks[IdUtil::getEntityIndex(ent)];
    for (auto& view : views) {
        if (view->mask.passes(mask)) { view->removeEntity(ent); }
    }
    ComponentMask::remove(mask, pool.ComponentIndex);
}

template<typename T>
ComponentPool<T>& Registry::getAllComponents() {
    return getPool<T>();
}

template<typename TRequire, typename TOptional>
ComponentSet<TRequire, TOptional> Registry::getComponentSet(Entity ent) {
    return ComponentSet<TRequire, TOptional>(*this, ent);
}

template<typename TRequire, typename TOptional, typename TExclude>
View<TRequire, TOptional, TExclude>* Registry::getOrCreateView() {
    using TView = View<TRequire, TOptional, TExclude>;

    // find existing view
    const std::type_index viewId = typeid(TView);
    for (auto& view : views) {
        if (view->id == viewId) return static_cast<TView*>(view.get());
    }

    // create new view
    views.emplace_back(new TView(*this));
    return static_cast<TView*>(views.back().get());
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
        it = poolMap.try_emplace(tid, new ComponentPool<T>(poolMap.size())).first;
        componentPools.emplace_back(it->second.get());
    }

    return *static_cast<ComponentPool<T>*>(componentPools[it->second->ComponentIndex]);
}

template<typename TRequire, typename TOptional, typename TExclude>
void Registry::populateView(View<TRequire, TOptional, TExclude>& view) {
    for (std::uint32_t i = 0; i < entityAllocator.endId(); ++i) {
        if (entityAllocator.isAllocated(i)) {
            if (view.mask.passes(entityMasks[i])) {
                view.tryAddEntity(IdUtil::composeEntity(i, entityVersions[i]));
            }
        }
    }
}

template<typename TRequire, typename TOptional, typename TExclude>
void Registry::populateViewWithLock(View<TRequire, TOptional, TExclude>& view) {
    std::lock_guard lock(entityLock);
    populateView(view);
}

} // namespace ecs
} // namespace bl

#endif
