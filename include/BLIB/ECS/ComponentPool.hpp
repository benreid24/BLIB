#ifndef BLIB_ECS_COMPONENTPOOL_HPP
#define BLIB_ECS_COMPONENTPOOL_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/ECS/Traits/ParentAware.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/ReadWriteLock.hpp>
#include <cstdlib>
#include <functional>
#include <limits>
#include <plf_colony.h>
#include <type_traits>
#include <utility>
#include <vector>

namespace bl
{
namespace ecs
{
class Registry;
template<typename TRequire, typename TOptional, typename TExclude>
class View;

/**
 * @brief Base class for component pools. Not intended to be used directly
 *
 * @ingroup ECS
 *
 */
class ComponentPoolBase : private util::NonCopyable {
public:
    /// @brief The 0-based index of this component pool
    const std::uint16_t ComponentIndex;

    /**
     * @brief Destroy the Component Pool Base object
     *
     */
    virtual ~ComponentPoolBase() = default;

protected:
    util::ReadWriteLock poolLock;

    ComponentPoolBase(std::uint16_t index)
    : ComponentIndex(index) {}

    virtual void remove(Entity entity) = 0;
    virtual void clear()               = 0;

    virtual void onParentSet(Entity child, Entity parent) = 0;
    virtual void onParentRemove(Entity orphan)            = 0;

    template<typename TRequire, typename TOptional, typename TExclude>
    friend class View;
    friend class Registry;
};

/**
 * @brief Provides storage and management for components in the ECS. Used internally in the Registry
 *
 * @tparam T The type of component to store
 * @ingroup ECS
 */
template<typename T>
class ComponentPool : public ComponentPoolBase {
public:
    /**
     * @brief Fetches the component for the given entity if it exists
     *
     * @param entity The entity to get the component for
     * @return T* Pointer to the component or nullptr if the entity does not have one
     */
    T* get(Entity entity);

    /**
     * @brief Iterates over all contained components and triggers the callback for each
     *
     * @tparam TCallback The callback type to invoke
     * @param cb The handler for each component. Takes the entity id and the component
     */
    template<typename TCallback>
    void forEach(const TCallback& cb);

    /**
     * @brief Iterates over all contained components and triggers the callback for each. Acquires a
     *        write lock on the pool. Use this if the callback may remove components, otherwise
     *        prefer forEach()
     *
     * @tparam TCallback The callback type to invoke
     * @param cb The handler for each component. Takes the entity id and the component
     */
    template<typename TCallback>
    void forEachWithWrites(const TCallback& cb);

private:
    static constexpr std::size_t DefaultCapacity = 64;
    static constexpr std::size_t InvalidIndex    = std::numeric_limits<std::size_t>::max();

    struct Entry {
        Entity owner;
        T component;

        template<typename... TArgs>
        Entry(Entity owner, TArgs&&... args)
        : owner(owner)
        , component(std::forward<TArgs>(args)...) {}
    };

    using TStorage = plf::colony<Entry>;

    TStorage storage;
    std::vector<T*> entityToComponent;
    std::vector<typename TStorage::iterator> entityToIter;

    ComponentPool(std::uint16_t index);
    virtual ~ComponentPool();

    void preAdd(Entity entity);
    void postAdd(Entity entity, TStorage::iterator it);
    T* add(Entity entity, const T& component);
    T* add(Entity entity, T&& component);
    template<typename... TArgs>
    T* emplace(Entity ent, TArgs&&... args);

    virtual void remove(Entity entity) override;
    virtual void clear() override;

    virtual void onParentSet(Entity child, Entity parent) override {
        if constexpr (std::is_base_of_v<trait::ParentAware<T>, T>) { setParent(child, parent); }
    }

    void setParent(Entity child, Entity parent) {
        T* childCom  = get(child);
        T* parentCom = get(parent);

        if (child && parent) { childCom->parent = parentCom; }
        else {
            if (!child) { BL_LOG_ERROR << "Invalid child entity: " << child; }
            else { BL_LOG_ERROR << "Invalid parent entity: " << parent; }
        }
    }

    virtual void onParentRemove(Entity orphan) override {
        if constexpr (std::is_base_of_v<trait::ParentAware<T>, T>) removeParent(orphan);
    }

    void removeParent(Entity orphan) {
        T* com = get(orphan);
        if (com) { com->parent = nullptr; }
        else { BL_LOG_WARN << "Invalid orphan entity: " << orphan; }
    }

    friend class Registry;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ComponentPool<T>::ComponentPool(std::uint16_t index)
: ComponentPoolBase(index)
, storage(plf::limits{128, TStorage::default_max_block_capacity()})
, entityToComponent(DefaultCapacity, nullptr)
, entityToIter(DefaultCapacity, storage.end()) {}

template<typename T>
ComponentPool<T>::~ComponentPool() {
    clear();
}

template<typename T>
void ComponentPool<T>::postAdd(Entity ent, TStorage::iterator it) {
    const std::uint64_t entIndex = IdUtil::getEntityIndex(ent);
    if (entIndex + 1 > entityToComponent.size()) {
        entityToComponent.resize(entIndex + 1, nullptr);
        entityToIter.resize(entIndex + 1, storage.end());
    }
    entityToComponent[entIndex] = &it->component;
    entityToIter[entIndex]      = it;
}

template<typename T>
void ComponentPool<T>::preAdd(Entity ent) {
    const std::uint64_t entIndex = IdUtil::getEntityIndex(ent);
    if (entIndex < entityToComponent.size()) {
        T* com = entityToComponent[entIndex];
        if (com) { storage.erase(entityToIter[entIndex]); }
    }
}

template<typename T>
T* ComponentPool<T>::add(Entity ent, const T& c) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    preAdd(ent);
    auto it = storage.emplace(ent, c);
    postAdd(ent, it);
    return &it->component;
}

template<typename T>
T* ComponentPool<T>::add(Entity ent, T&& c) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    preAdd(ent);
    auto it = storage.emplace(ent, std::forward<T>(c));
    postAdd(ent, it);
    return &it->component;
}

template<typename T>
template<typename... TArgs>
T* ComponentPool<T>::emplace(Entity ent, TArgs&&... args) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    preAdd(ent);
    auto it = storage.emplace(ent, std::forward<TArgs>(args)...);
    postAdd(ent, it);
    return &it->component;
}

template<typename T>
void ComponentPool<T>::remove(Entity ent) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    // determine if present
    const std::uint64_t entIndex = IdUtil::getEntityIndex(ent);
    if (entIndex >= entityToComponent.size()) return;
    T* com = entityToComponent[entIndex];
    if (com == nullptr) return;

    // send event
    bl::event::Dispatcher::dispatch<event::ComponentRemoved<T>>({ent, *com});

    // perform removal
    storage.erase(entityToIter[entIndex]);
    entityToComponent[entIndex] = nullptr;
}

template<typename T>
void ComponentPool<T>::clear() {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    // send events
    for (Entry& entry : storage) {
        bl::event::Dispatcher::dispatch<event::ComponentRemoved<T>>({entry.owner, entry.component});
    }

    // clear storage
    storage.clear();

    // reset metadata
    std::fill(entityToComponent.begin(), entityToComponent.end(), nullptr);
}

template<typename T>
template<typename TCallback>
void ComponentPool<T>::forEach(const TCallback& cb) {
    static_assert(std::is_invocable<TCallback, Entity, T&>::value,
                  "Visitor signature is void(Entity, T&)");

    util::ReadWriteLock::ReadScopeGuard lock(poolLock);
    for (Entry& entry : storage) { cb(entry.owner, entry.component); }
}

template<typename T>
template<typename TCallback>
void ComponentPool<T>::forEachWithWrites(const TCallback& cb) {
    static_assert(std::is_invocable<TCallback, Entity, T&>::value,
                  "Visitor signature is void(Entity, T&)");

    util::ReadWriteLock::WriteScopeGuard lock(poolLock);
    for (Entry& entry : storage) { cb(entry.owner, entry.component); }
}

template<typename T>
T* ComponentPool<T>::get(Entity ent) {
    util::ReadWriteLock::ReadScopeGuard lock(poolLock);

    const std::uint64_t entIndex = IdUtil::getEntityIndex(ent);
    if (entIndex < entityToComponent.size()) { return entityToComponent[entIndex]; }
    return nullptr;
}

} // namespace ecs
} // namespace bl

#endif
