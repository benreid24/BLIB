#ifndef BLIB_ECS_COMPONENTPOOL_HPP
#define BLIB_ECS_COMPONENTPOOL_HPP

#include <BLIB/Containers/RingQueue.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/ReadWriteLock.hpp>
#include <cstdlib>
#include <functional>
#include <limits>
#include <type_traits>
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
    const unsigned int ComponentIndex;

    /**
     * @brief Destroy the Component Pool Base object
     *
     */
    virtual ~ComponentPoolBase() = default;

    /**
     * @brief Returns the number of different component types that exist
     *
     * @return unsigned int The number of unique component types
     */
    static unsigned int ComponentCount();

protected:
    util::ReadWriteLock poolLock;

    ComponentPoolBase()
    : ComponentIndex(nextComponentIndex++) {}

    virtual void remove(Entity entity) = 0;
    virtual void clear()               = 0;

    static unsigned int nextComponentIndex;

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

private:
    std::vector<container::ObjectWrapper<T>> pool;
    std::vector<typename std::vector<container::ObjectWrapper<T>>::iterator> entityToIter;
    std::vector<Entity> indexToEntity;
    std::vector<bool> aliveIndices;
    container::RingQueue<std::uint16_t> freeIndices;
    std::uint16_t nextIndex;
    std::uint8_t maxIndex;

    ComponentPool(std::size_t poolSize);
    static ComponentPool& get(std::size_t poolSize);

    typename std::vector<container::ObjectWrapper<T>>::iterator addLogic(Entity entity);
    T* add(Entity entity, const T& component);
    T* add(Entity entity, T&& component);
    template<typename... TArgs>
    T* emplace(Entity ent, TArgs... args);

    virtual void remove(Entity entity) override;
    virtual void clear() override;

    friend class Registry;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ComponentPool<T>::ComponentPool(std::size_t ps)
: ComponentPoolBase()
, pool(ps)
, entityToIter(ps, pool.end())
, indexToEntity(ps, InvalidEntity)
, aliveIndices(ps, false)
, freeIndices(ps)
, nextIndex(0) {}

template<typename T>
ComponentPool<T>& ComponentPool<T>::get(std::size_t ps) {
    static ComponentPool<T> pool(ps);
    return pool;
}

template<typename T>
typename std::vector<container::ObjectWrapper<T>>::iterator ComponentPool<T>::addLogic(Entity ent) {
    // determine insertion index
    std::uint16_t i = nextIndex;
    if (!freeIndices.empty()) {
        i = freeIndices.front();
        freeIndices.pop();
    }
    else {
        ++nextIndex;
        maxIndex = i;
    }

    // check not full
    if (i >= pool.size()) {
        nextIndex = pool.size(); // dont break iteration
        BL_LOG_CRITICAL << "Ran out of storage in component pool. Increase allocation. Capacity: "
                        << pool.size() << " Pool: " << typeid(T).name();
        std::exit(1);
    }

    // perform insertion
    auto it           = pool.begin() + i;
    entityToIter[ent] = it;
    indexToEntity[i]  = ent;
    aliveIndices[i]   = true;

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
T* ComponentPool<T>::emplace(Entity ent, TArgs... args) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    auto it = addLogic(ent);
    it->emplace(args...);
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
    aliveIndices[i]   = false;
    freeIndices.push(i);

    // push back maxIndex to keep iterations tight
    if (i == maxIndex) {
        do { --maxIndex; } while (maxIndex > 0 && !aliveIndices[maxIndex - 1]);
    }
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
    auto entIt   = indexToEntity.begin();
    auto aliveIt = aliveIndices.begin();
    while (entIt != indexToEntity.end() && aliveIt != aliveIndices.end()) {
        *entIt   = InvalidEntity;
        *aliveIt = false;
        ++entIt;
        ++aliveIt;
    }
    freeIndices.clear();
    nextIndex = 0;
    maxIndex  = 0;
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
    auto aliveIt    = aliveIndices.begin();
    while (i <= maxIndex) {
        if (*aliveIt) { cb(*entIt, poolIt->get()); }

        ++i;
        ++poolIt;
        ++entIt;
        ++aliveIt;
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
