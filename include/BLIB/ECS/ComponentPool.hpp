#ifndef BLIB_ECS_COMPONENTPOOL_HPP
#define BLIB_ECS_COMPONENTPOOL_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/ReadWriteLock.hpp>
#include <cstdlib>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace bl
{
namespace ecs
{
class Registry;
template<typename... TComponents>
class View;

/**
 * @brief Base class for component pools. Not intended to be used directly
 *
 * @ingroup ECS
 *
 */
class ComponentPoolBase : private util::NonCopyable {
public:
    /// @brief The maximum number of components a pool can hold
    static constexpr std::size_t MaximumComponentCount = std::numeric_limits<std::uint16_t>::max();

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

    template<typename... TComponents>
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
    std::vector<container::ObjectWrapper<T>> pool;
    std::vector<typename std::vector<container::ObjectWrapper<T>>::iterator> entityToIter;
    std::vector<Entity> indexToEntity;
    util::IdAllocator<std::uint16_t> indexAllocator;

    ComponentPool(std::uint16_t index, std::size_t poolSize);

    typename std::vector<container::ObjectWrapper<T>>::iterator addLogic(Entity entity);
    T* add(Entity entity, const T& component);
    T* add(Entity entity, T&& component);
    template<typename... TArgs>
    T* emplace(Entity ent, TArgs&&... args);

    virtual void remove(Entity entity) override;
    virtual void clear() override;

    friend class Registry;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ComponentPool<T>::ComponentPool(std::uint16_t index, std::size_t ps)
: ComponentPoolBase(index)
, pool(ps)
, entityToIter(ps, pool.end())
, indexToEntity(ps, InvalidEntity)
, indexAllocator(ps) {}

template<typename T>
typename std::vector<container::ObjectWrapper<T>>::iterator ComponentPool<T>::addLogic(Entity ent) {
    // check not full
    if (!indexAllocator.available()) {
        BL_LOG_CRITICAL << "Ran out of storage in component pool. Increase allocation. Capacity: "
                        << pool.size() << " Pool: " << typeid(T).name();
        std::exit(1);
    }

    // perform insertion
    const std::uint16_t i = indexAllocator.allocate();
    auto it               = pool.begin() + i;
    entityToIter[ent]     = it;
    indexToEntity[i]      = ent;

    return it;
}

template<typename T>
T* ComponentPool<T>::add(Entity ent, const T& c) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    auto it = addLogic(ent);
    it->emplace(c);
    return &it->get();
}

template<typename T>
T* ComponentPool<T>::add(Entity ent, T&& c) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    auto it = addLogic(ent);
    it->emplace(c);
    return &it->get();
}

template<typename T>
template<typename... TArgs>
T* ComponentPool<T>::emplace(Entity ent, TArgs&&... args) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    auto it = addLogic(ent);
    it->emplace(std::forward<TArgs>(args)...);
    return &it->get();
}

template<typename T>
void ComponentPool<T>::remove(Entity ent) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    // determine if present
    auto it = entityToIter[ent];
    if (it == pool.end()) return;

    // send event
    bl::event::Dispatcher::dispatch<event::ComponentRemoved<T>>({ent, it->get()});

    // perform removal
    const std::uint16_t i = it - pool.begin();
    it->destroy();
    entityToIter[ent] = pool.end();
    indexToEntity[i]  = InvalidEntity;
    indexAllocator.release(i);
}

template<typename T>
void ComponentPool<T>::clear() {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    // destroy components
    Entity ent = 0;
    for (auto& it : entityToIter) {
        if (it != pool.end()) {
            bl::event::Dispatcher::dispatch<event::ComponentRemoved<T>>({ent, it->get()});
            it->destroy();
            it = pool.end();
        }
        ++ent;
    }

    // reset metadata
    for (auto entIt = indexToEntity.begin(); entIt != indexToEntity.end(); ++entIt) {
        *entIt = InvalidEntity;
    }
    indexAllocator.releaseAll();
}

template<typename T>
template<typename TCallback>
void ComponentPool<T>::forEach(const TCallback& cb) {
    static_assert(std::is_invocable<TCallback, Entity, T&>::value,
                  "Visitor signature is void(Entity, T&)");

    util::ReadWriteLock::ReadScopeGuard lock(poolLock);

    std::uint16_t i = 0;
    auto poolIt     = pool.begin();
    auto entIt      = indexToEntity.begin();
    while (i <= indexAllocator.highestId()) {
        if (indexAllocator.isAllocated(i)) { cb(*entIt, poolIt->get()); }

        ++i;
        ++poolIt;
        ++entIt;
    }
}

template<typename T>
template<typename TCallback>
void ComponentPool<T>::forEachWithWrites(const TCallback& cb) {
    static_assert(std::is_invocable<TCallback, Entity, T&>::value,
                  "Visitor signature is void(Entity, T&)");

    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    std::uint16_t i = 0;
    auto poolIt     = pool.begin();
    auto entIt      = indexToEntity.begin();
    while (i <= indexAllocator.highestId()) {
        if (indexAllocator.isAllocated(i)) { cb(*entIt, poolIt->get()); }

        ++i;
        ++poolIt;
        ++entIt;
    }
}

template<typename T>
T* ComponentPool<T>::get(Entity ent) {
    util::ReadWriteLock::ReadScopeGuard lock(poolLock);

    auto it = entityToIter[ent];
    return it != pool.end() ? &it->get() : nullptr;
}

} // namespace ecs
} // namespace bl

#endif
