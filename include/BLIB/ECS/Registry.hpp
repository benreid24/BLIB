#ifndef BLIB_ECS_REGISTRY_HPP
#define BLIB_ECS_REGISTRY_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/DependencyGraph.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/ParentDestructionBehavior.hpp>
#include <BLIB/ECS/ParentGraph.hpp>
#include <BLIB/ECS/Tags.hpp>
#include <BLIB/ECS/Transaction.hpp>
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
class Registry : private util::NonCopyable {
public:
    static constexpr std::size_t DefaultCapacity = 512;

    /**
     * @brief Creates a new entity registry
     */
    Registry();

    /**
     * @brief Creates a new entity and returns its id
     *
     * @param worldIndex The index of the world that the entity is in
     * @param flags Optional flags to set on the newly created entity. May not be modified later
     * @return Entity The new entity id
     */
    Entity createEntity(unsigned int worldIndex, Flags flags = Flags::None);

    /**
     * @brief Creates a new entity and returns its id
     *
     * @param worldIndex The index of the world that the entity is in
     * @param flags Optional flags to set on the newly created entity. May not be modified later
     * @param transaction The transaction to use to synchronize access
     * @return Entity The new entity id
     */
    Entity createEntity(unsigned int worldIndex, Flags flags,
                        const Transaction<tx::EntityWrite>& transaction);

    /**
     * @brief Returns whether or not the given entity exists
     *
     * @param entity The entity to test for
     * @return True if the entity is in the ECS, false otherwise
     */
    bool entityExists(Entity entity) const;

    /**
     * @brief Returns whether or not the given entity exists
     *
     * @param entity The entity to test for
     * @return True if the entity is in the ECS, false otherwise
     * @param transaction The transaction to use to synchronize access
     */
    bool entityExists(Entity entity, const Transaction<tx::EntityRead>& transaction) const;

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
     * @brief Removes the given entity and destroys all of its components. Destruction will not
     *        occur if the entity is depended on by others. In that case it will be marked for later
     *        removal once the dependencies are removed
     *
     * @param entity The entity to remove from the ECS
     * @param transaction The transaction to use to synchronize access
     * @return True if the entity was removed or was already destroyed, false if removal was blocked
     */
    bool destroyEntity(Entity entity, const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Destroys all entities that have the given flags
     *
     * @param flags The flags to search for
     * @return The number of destroyed entities, not counting freed resource entities
     */
    unsigned int destroyAllEntitiesWithFlags(Flags flags);

    /**
     * @brief Destroys all entities that have the given flags
     *
     * @param flags The flags to search for
     * @param transaction The transaction to use to synchronize access
     * @return The number of destroyed entities, not counting freed resource entities
     */
    unsigned int destroyAllEntitiesWithFlags(Flags flags,
                                             const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Destroys all entities in the given world
     *
     * @param index The index of the world to purge entities from
     * @return The number of destroyed entities
     */
    unsigned int destroyEntitiesInWorld(unsigned int index);

    /**
     * @brief Destroys all entities in the given world
     *
     * @param index The index of the world to purge entities from
     * @param transaction The transaction to use to synchronize access
     * @return The number of destroyed entities
     */
    unsigned int destroyEntitiesInWorld(unsigned int index,
                                        const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Destroys all entities with the WorldObject flag
     *
     * @return The number of destroyed entities, not counting freed resource entities
     */
    unsigned int destroyAllWorldEntities();

    /**
     * @brief Destroys all entities with the WorldObject flag
     *
     * @param transaction The transaction to use to synchronize access
     * @return The number of destroyed entities, not counting freed resource entities
     */
    unsigned int destroyAllWorldEntities(const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Destroys and removes all entities and components from the ECS
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
     * @brief Sets the parent entity of the given entity
     *
     * @param child The entity to set the parent of
     * @param parent The parent of the given entity
     * @param transaction The transaction to use to synchronize access
     */
    void setEntityParent(Entity child, Entity parent,
                         const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Removes the parent of the given entity, if any
     *
     * @param child The entity to orphan
     */
    void removeEntityParent(Entity child);

    /**
     * @brief Removes the parent of the given entity, if any
     *
     * @param child The entity to orphan
     * @param transaction The transaction to use to synchronize access
     */
    void removeEntityParent(Entity child, const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Returns whether or not the given entity has a parent
     *
     * @param child The entity to test for a parent
     * @return True if child has a parent, false otherwise
     */
    bool entityHasParent(Entity child) const;

    /**
     * @brief Returns whether or not the given entity has a parent
     *
     * @param child The entity to test for a parent
     * @return True if child has a parent, false otherwise
     * @param transaction The transaction to use to synchronize access
     */
    bool entityHasParent(Entity child, const Transaction<tx::EntityRead>& transaction) const;

    /**
     * @brief Returns the entity id of the parent of the given entity if one is set
     *
     * @param child The entity to get the parent for
     * @return The parent entity or InvalidEntity if no parent
     */
    Entity getEntityParent(Entity child) const;

    /**
     * @brief Returns the entity id of the parent of the given entity if one is set
     *
     * @param child The entity to get the parent for
     * @return The parent entity or InvalidEntity if no parent
     * @param transaction The transaction to use to synchronize access
     */
    Entity getEntityParent(Entity child, const Transaction<tx::EntityRead>& transaction) const;

    /**
     * @brief Returns an iterable set of child entities for the given entity
     *
     * @param parent The entity to get the children of
     * @return An iterable range of child entities
     */
    ctr::IndexMappedList<std::uint32_t, Entity>::Range getEntityChildren(Entity parent);

    /**
     * @brief Sets what to do for the given entity when any of its parents are destroyed
     *
     * @param entity The entity to set the behavior for
     * @param behavior What to do when a parent is destroyed
     */
    void setEntityParentDestructionBehavior(Entity entity, ParentDestructionBehavior behavior);

    /**
     * @brief Returns the behavior of the entity when its parent is destroyed
     *
     * @param entity The entity to get the behavior for
     * @return What the entity does when its parent is destroyed
     */
    ParentDestructionBehavior getEntityParentDestructionBehavior(Entity entity) const;

    /**
     * @brief Adds a dependency on resource from user. Controls whether or not an entity may be
     *        safely deleted
     *
     * @param resource The entity being depended on
     * @param user The entity using the resource
     */
    void addDependency(Entity resource, Entity user);

    /**
     * @brief Adds a dependency on resource from user. Controls whether or not an entity may be
     *        safely deleted
     *
     * @param resource The entity being depended on
     * @param user The entity using the resource
     * @param transaction The transaction to use to synchronize access
     */
    void addDependency(Entity resource, Entity user,
                       const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Removes the given dependency. Will destroy the resource if a prior call to
     *        destroyEntity() failed for it
     *
     * @param resource The entity being depended on
     * @param user The entity using the resource
     */
    void removeDependency(Entity resource, Entity user);

    /**
     * @brief Removes the given dependency. Will destroy the resource if a prior call to
     *        destroyEntity() failed for it
     *
     * @param resource The entity being depended on
     * @param user The entity using the resource
     * @param transaction The transaction to use to synchronize access
     */
    void removeDependency(Entity resource, Entity user,
                          const Transaction<tx::EntityRead>& transaction);

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
     * @brief Calls removeDependency() and then destroys the resource if nothing else depends on it.
     *        If it still has dependencies then it will be marked for removal once all dependencies
     *        are removed
     *
     * @param resource The resource being used
     * @param user The entity using the resource
     * @param transaction The transaction to use to synchronize access
     */
    void removeDependencyAndDestroyIfPossible(Entity resource, Entity user,
                                              const Transaction<tx::EntityRead>& transaction);

    /**
     * @brief Returns whether or not the given entity is depended on by other entities
     *
     * @param resource The entity to check
     * @return True if other entities depend on it, false otherwise
     */
    bool isDependedOn(Entity resource) const;

    /**
     * @brief Returns whether or not the given entity is depended on by other entities
     *
     * @param resource The entity to check
     * @param transaction The transaction to use to synchronize access
     * @return True if other entities depend on it, false otherwise
     */
    bool isDependedOn(Entity resource, const Transaction<tx::EntityRead>& transaction) const;

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
     * @param transaction The transaction to use to synchronize access
     * @return T* Pointer to the new component
     */
    template<typename T>
    T* addComponent(
        Entity entity, const T& value,
        const Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>& transaction);

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
     * @brief Adds a new component of the given type to the given entity
     *
     * @tparam T The type of component to add
     * @param entity The entity to add it to
     * @param value The initial component value
     * @param transaction The transaction to use to synchronize access
     * @return T* Pointer to the new component
     */
    template<typename T>
    T* addComponent(
        Entity entity, T&& value,
        const Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>& transaction);

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
     * @brief Constructs the given component in-place for the given entity
     *
     * @tparam T The type of component to add
     * @tparam TArgs The constructor parameter types
     * @param entity The entity to add it to
     * @param args The arguments to the component's constructor
     * @param transaction The transaction to use to synchronize access
     * @return T* Pointer to the new component
     */
    template<typename T, typename... TArgs>
    T* emplaceComponent(
        Entity entity, TArgs&&... args,
        const Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>& transaction);

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
     * @brief Retrieves the given component belonging to the given entity
     *
     * @tparam T The type of component to get
     * @param entity The entity to get the component for
     * @param transaction The transaction to use to synchronize access
     * @return T* Pointer to the entity's component or nullptr if not present
     */
    template<typename T>
    T* getComponent(Entity entity, const txp::TransactionComponentRead<T>& transaction);

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
     * @brief Returns whether or not the entity has the given component
     *
     * @tparam T The component type to check for
     * @param entity The entity to check on
     * @param transaction The transaction to use to synchronize access
     * @return True if the entity has the component, false otherwise
     */
    template<typename T>
    bool hasComponent(Entity entity, const txp::TransactionComponentRead<T>& transaction);

    /**
     * @brief Removes the given component from the given entity
     *
     * @tparam T The type of component to remove
     * @param entity The entity to remove the component from
     */
    template<typename T>
    void removeComponent(Entity entity);

    /**
     * @brief Removes the given component from the given entity
     *
     * @tparam T The type of component to remove
     * @param entity The entity to remove the component from
     * @param transaction The transaction to use to synchronize access
     */
    template<typename T>
    void removeComponent(Entity entity, const txp::TransactionComponentWrite<T>& transaction);

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
     * @brief Gets a set of components for the given entity
     *
     * @tparam TRequire Required tagged components. ie Require<int, char>
     * @tparam TOptional Optional tagged components. ie Optional<bool>
     * @param entity The entity to get the components from
     * @param transaction The transaction to use to synchronize access
     * @return ComponentSet<TComponents...> The set of fetched components. May be invalid
     */
    template<typename TRequire, typename TOptional = Optional<>>
    ComponentSet<TRequire, TOptional> getComponentSet(
        Entity entity,
        const Transaction<tx::EntityUnlocked, typename Tags<TRequire, TOptional>::ReadTx>&
            transaction);

    /**
     * @brief Fetches or creates a view that returns an iterable set of entities that contain the
     *        given components
     *
     * @tparam TComponents The components to filter on
     * @return View<TComponents...>* A pointer to a view that returns all matching entities
     */
    template<typename TRequire, typename TOptional = Optional<>, typename TExclude = Exclude<>>
    View<TRequire, TOptional, TExclude>* getOrCreateView();

    /**
     * @brief Deletes all entities and components currently queued for removal
     */
    void flushDeletions();

private:
    // entity id management
    mutable std::recursive_mutex entityLock;
    util::IdAllocatorUnbounded<std::uint32_t> entityAllocator;
    std::vector<ComponentMask::SimpleMask> entityMasks;
    std::vector<std::uint16_t> entityVersions;
    std::vector<Flags> entityFlags;
    std::vector<std::uint8_t> entityWorlds;

    // entity relationships
    ParentGraph parentGraph;
    std::vector<ParentDestructionBehavior> parentDestructionBehaviors;
    DependencyGraph dependencyGraph;
    std::vector<bool> markedForRemoval;

    // components
    std::vector<ComponentPoolBase*> componentPools;
    std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> poolMap;

    // views
    std::mutex viewMutex;
    std::vector<std::unique_ptr<priv::ViewBase>> views;

    // deletion traversal & queue
    struct {
        std::recursive_mutex mutex;
        std::vector<Entity> toVisit;
        std::vector<Entity> toUnparent;
        std::vector<Entity> toRemove;
    } deletionState;

    template<typename T>
    ComponentPool<T>& getPool();

    bool entityExistsLocked(Entity ent) const;
    void markEntityForRemoval(Entity ent);
    void doEntityDestroyLocked(Entity ent);
    bool queueEntityDestroy(Entity ent);

    template<typename TRequire, typename TOptional, typename TExclude>
    void populateView(View<TRequire, TOptional, TExclude>& view);
    template<typename TRequire, typename TOptional, typename TExclude>
    void populateViewWithLock(View<TRequire, TOptional, TExclude>& view);

    template<typename T>
    void finishComponentAdd(Entity ent, unsigned int cindex, T* component);

    void removeEntityParentLocked(Entity child);

    template<typename TRequire, typename TOptional, typename TExclude>
    friend class View;
    friend class engine::Engine;

    template<typename T>
    friend class txp::TransactionComponentRead;
    template<typename T>
    friend class txp::TransactionComponentWrite;
    friend class txp::TransactionEntityRead;
    friend class txp::TransactionEntityWrite;
};

} // namespace ecs
} // namespace bl

#include <BLIB/ECS/ComponentSetImpl.hpp>
#include <BLIB/ECS/TagsImpl.hpp>
#include <BLIB/ECS/TransactionImpl.hpp>
#include <BLIB/ECS/ViewImpl.hpp>

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

namespace bl
{
namespace ecs
{
template<typename T>
T* Registry::addComponent(Entity ent, const T& val) {
    return addComponent<T>(
        ent, val, Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>(*this));
}

template<typename T>
T* Registry::addComponent(
    Entity entity, const T& value,
    const Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>&) {
    auto& pool = getPool<T>();
    T* nc      = pool.add(entity, value);
    finishComponentAdd<T>(entity, pool.ComponentIndex, nc);
    return nc;
}

template<typename T>
T* Registry::addComponent(Entity ent, T&& val) {
    return addComponent<T>(
        ent,
        std::forward<T>(val),
        Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>(*this));
}

template<typename T>
T* Registry::addComponent(
    Entity entity, T&& value,
    const Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>&) {
    auto& pool = getPool<T>();
    T* nc      = pool.add(entity, value);
    finishComponentAdd<T>(entity, pool.ComponentIndex, nc);
    return nc;
}

template<typename T, typename... TArgs>
T* Registry::emplaceComponent(Entity ent, TArgs&&... args) {
    return emplaceComponent<T>(
        ent,
        std::forward<TArgs>(args)...,
        Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>(*this));
}

template<typename T, typename... TArgs>
inline T* Registry::emplaceComponent(
    Entity entity, TArgs&&... args,
    const Transaction<tx::EntityRead, tx::ComponentRead<>, tx::ComponentWrite<T>>& transaction) {
    auto& pool = getPool<T>();
    T* nc      = pool.emplace(entity, std::forward<TArgs>(args)...);
    finishComponentAdd<T>(entity, pool.ComponentIndex, nc);
    return nc;
}

template<typename T>
void Registry::finishComponentAdd(Entity ent, unsigned int cIndex, T* component) {
    bl::event::Dispatcher::dispatch<event::ComponentAdded<T>>({ent, *component});
    ComponentMask::SimpleMask& mask        = entityMasks[ent.getIndex()];
    const ComponentMask::SimpleMask ogMask = mask;
    ComponentMask::add(mask, cIndex);
    for (auto& view : views) {
        if (view->mask.passes(mask)) { view->tryAddEntity(ent); }
        else if (view->mask.passes(ogMask) && ComponentMask::has(view->mask.excluded, cIndex)) {
            view->removeEntity(ent);
        }
    }

    // ensure parent traits are up to date
    auto& pool = getPool<T>();
    for (Entity child : parentGraph.getChildren(ent)) {
        const std::uint64_t ic = child.getIndex();
        const ComponentMask mask{.required = entityMasks[ic]};
        if (mask.contains(pool.ComponentIndex)) { pool.onParentSet(child, ent); }
    }
    const Entity parent = parentGraph.getParent(ent);
    if (parent != InvalidEntity) {
        const std::uint64_t ip = parent.getIndex();
        const ComponentMask mask{.required = entityMasks[ip]};
        if (mask.contains(pool.ComponentIndex)) { pool.onParentSet(ent, parent); }
    }
}

template<typename T>
T* Registry::getComponent(Entity ent) {
    return getPool<T>().get(ent);
}

template<typename T>
T* Registry::getComponent(Entity entity, const txp::TransactionComponentRead<T>& transaction) {
    return getPool<T>().get(entity, transaction);
}

template<typename T>
bool Registry::hasComponent(Entity ent) {
    return getComponent<T>(ent) != nullptr;
}

template<typename T>
bool Registry::hasComponent(Entity entity, const txp::TransactionComponentRead<T>& transaction) {
    return getComponent<T>(entity, transaction) != nullptr;
}

template<typename T>
void Registry::removeComponent(Entity ent) {
    return removeComponent<T>(ent, txp::TransactionComponentWrite<T>(*this));
}

template<typename T>
void Registry::removeComponent(Entity ent, const txp::TransactionComponentWrite<T>&) {
    if (!entityExistsLocked(ent)) { return; }

    auto& pool   = getPool<T>();
    const auto i = ent.getIndex();
    if (i >= entityMasks.size()) { return; }
    ComponentMask::SimpleMask& mask = entityMasks[i];
    if (!ComponentMask::contains(mask, pool.ComponentIndex)) { return; }

    // notify pools of parent remove
    for (Entity child : parentGraph.getChildren(ent)) {
        const std::uint32_t ic = child.getIndex();
        const ComponentMask mask{.required = entityMasks[ic]};
        if (mask.contains(pool.ComponentIndex)) { pool.onParentRemove(ent, child); }
    }
    const Entity parent = parentGraph.getParent(ent);
    if (parent != InvalidEntity) {
        const std::uint32_t ip = parent.getIndex();
        const ComponentMask mask{.required = entityMasks[ip]};
        if (mask.contains(pool.ComponentIndex)) { pool.onParentRemove(parent, ent); }
    }

    // do remove
    void* com = pool.queueRemove(ent);
    for (auto& view : views) {
        if (ComponentMask::contains(view->mask.required, pool.ComponentIndex)) {
            view->removeEntity(ent);
        }
        else if (ComponentMask::contains(view->mask.optional, pool.ComponentIndex)) {
            view->nullEntityComponent(ent, com);
        }
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

template<typename TRequire, typename TOptional>
inline ComponentSet<TRequire, TOptional> Registry::getComponentSet(
    Entity entity,
    const Transaction<tx::EntityUnlocked, typename Tags<TRequire, TOptional>::ReadTx>&
        transaction) {
    return ComponentSet<TRequire, TOptional>(*this, entity, transaction);
}

template<typename TRequire, typename TOptional, typename TExclude>
View<TRequire, TOptional, TExclude>* Registry::getOrCreateView() {
    using TView = View<TRequire, TOptional, TExclude>;
    std::unique_lock lock(viewMutex);

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
        it = poolMap.try_emplace(tid, new ComponentPool<T>(*this, poolMap.size())).first;
        componentPools.emplace_back(it->second.get());
    }

    return static_cast<ComponentPool<T>&>(*it->second);
}

template<typename TRequire, typename TOptional, typename TExclude>
void Registry::populateView(View<TRequire, TOptional, TExclude>& view) {
    for (std::uint32_t i = 0; i < entityAllocator.endId(); ++i) {
        if (entityAllocator.isAllocated(i)) {
            if (view.mask.passes(entityMasks[i])) {
                view.tryAddEntity(Entity(i, entityVersions[i]));
            }
        }
    }
}

template<typename TRequire, typename TOptional, typename TExclude>
void Registry::populateViewWithLock(View<TRequire, TOptional, TExclude>& view) {
    std::lock_guard lock(entityLock);
    populateView(view);
}

inline bool Registry::entityHasParent(Entity child) const {
    return entityHasParent(child, Transaction<tx::EntityRead>(*this));
}

inline bool Registry::entityHasParent(Entity child, const Transaction<tx::EntityRead>&) const {
    return parentGraph.getParent(child) != InvalidEntity;
}

inline Entity Registry::getEntityParent(Entity child) const {
    return getEntityParent(child, Transaction<tx::EntityRead>(*this));
}

inline Entity Registry::getEntityParent(Entity child, const Transaction<tx::EntityRead>&) const {
    return parentGraph.getParent(child);
}

inline ctr::IndexMappedList<std::uint32_t, Entity>::Range Registry::getEntityChildren(
    Entity parent) {
    return parentGraph.getChildren(parent);
}

inline ParentDestructionBehavior Registry::getEntityParentDestructionBehavior(Entity entity) const {
    const std::uint64_t i = entity.getIndex();
    return i < parentDestructionBehaviors.size() ? parentDestructionBehaviors[i] :
                                                   ParentDestructionBehavior::DestroyedWithParent;
}

} // namespace ecs
} // namespace bl

#endif
