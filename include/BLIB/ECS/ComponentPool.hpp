#ifndef BLIB_ECS_COMPONENTPOOL_HPP
#define BLIB_ECS_COMPONENTPOOL_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/IdAllocatorUnbounded.hpp>
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

    std::vector<ctr::ObjectWrapper<T>> pool;
    std::vector<std::size_t> entityToIndex;
    std::vector<Entity> indexToEntity;
    util::IdAllocatorUnbounded<std::size_t> indexAllocator;

    ComponentPool(std::uint16_t index);
    virtual ~ComponentPool();

    std::size_t addLogic(Entity entity);
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
ComponentPool<T>::ComponentPool(std::uint16_t index)
: ComponentPoolBase(index)
, pool(DefaultCapacity)
, entityToIndex(DefaultCapacity, InvalidIndex)
, indexToEntity(DefaultCapacity, InvalidEntity)
, indexAllocator(DefaultCapacity) {}

template<typename T>
ComponentPool<T>::~ComponentPool() {
    clear();
}

template<typename T>
std::size_t ComponentPool<T>::addLogic(Entity ent) {
    const std::uint64_t entIndex = IdUtil::getEntityIndex(ent);
    if (entIndex + 1 > entityToIndex.size()) { entityToIndex.resize(entIndex + 1, InvalidIndex); }

    // prevent duplicate add
    const std::size_t existing = entityToIndex[entIndex];
    if (existing != InvalidIndex) { return existing; }

    // perform insertion
    const std::size_t i = indexAllocator.allocate();
    if (i + 1 > pool.size()) {
        pool.resize(i + 1);
        indexToEntity.resize(i + 1, InvalidEntity);
        bl::event::Dispatcher::dispatch<event::ComponentPoolResized>({ComponentIndex});
    }
    entityToIndex[entIndex] = i;
    indexToEntity[i]        = ent;

    return i;
}

template<typename T>
T* ComponentPool<T>::add(Entity ent, const T& c) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    auto& slot = pool[addLogic(ent)];
    slot.emplace(c);
    return &slot.get();
}

template<typename T>
T* ComponentPool<T>::add(Entity ent, T&& c) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    auto& slot = pool[addLogic(ent)];
    slot.emplace(std::forward<T>(c));
    return &slot.get();
}

template<typename T>
template<typename... TArgs>
T* ComponentPool<T>::emplace(Entity ent, TArgs&&... args) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    auto& slot = pool[addLogic(ent)];
    slot.emplace(std::forward<TArgs>(args)...);
    return &slot.get();
}

template<typename T>
void ComponentPool<T>::remove(Entity ent) {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    // determine if present
    const std::uint64_t entIndex = IdUtil::getEntityIndex(ent);
    if (entIndex >= entityToIndex.size()) return;
    const auto index = entityToIndex[entIndex];
    if (index == InvalidIndex) return;

    // send event
    auto& slot = pool[index];
    bl::event::Dispatcher::dispatch<event::ComponentRemoved<T>>({ent, slot.get()});

    // perform removal
    slot.destroy();
    entityToIndex[entIndex] = InvalidIndex;
    indexToEntity[index]    = InvalidEntity;
    indexAllocator.release(index);
}

template<typename T>
void ComponentPool<T>::clear() {
    util::ReadWriteLock::WriteScopeGuard lock(poolLock);

    // destroy components
    for (std::size_t i = 0; i < indexAllocator.poolSize(); ++i) {
        if (indexAllocator.isAllocated(i)) {
            auto& slot = pool[i];
            bl::event::Dispatcher::dispatch<event::ComponentRemoved<T>>(
                {indexToEntity[i], slot.get()});
            pool[i].destroy();
        }
    }

    // reset metadata
    std::fill(indexToEntity.begin(), indexToEntity.end(), InvalidEntity);
    std::fill(entityToIndex.begin(), entityToIndex.end(), InvalidIndex);
    indexAllocator.releaseAll();
}

template<typename T>
template<typename TCallback>
void ComponentPool<T>::forEach(const TCallback& cb) {
    static_assert(std::is_invocable<TCallback, Entity, T&>::value,
                  "Visitor signature is void(Entity, T&)");

    util::ReadWriteLock::ReadScopeGuard lock(poolLock);

    std::size_t i = 0;
    auto poolIt   = pool.begin();
    auto entIt    = indexToEntity.begin();
    while (i < indexAllocator.endId()) {
        if (*entIt != InvalidEntity) { cb(*entIt, poolIt->get()); }

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

    std::size_t i = 0;
    auto poolIt   = pool.begin();
    auto entIt    = indexToEntity.begin();
    while (i < indexAllocator.endId()) {
        if (*entIt != InvalidEntity) { cb(*entIt, poolIt->get()); }

        ++i;
        ++poolIt;
        ++entIt;
    }
}

template<typename T>
T* ComponentPool<T>::get(Entity ent) {
    util::ReadWriteLock::ReadScopeGuard lock(poolLock);

    const std::uint64_t entIndex = IdUtil::getEntityIndex(ent);
    if (entIndex < entityToIndex.size()) {
        if (entityToIndex[entIndex] != InvalidIndex) {
            return &pool[entityToIndex[entIndex]].get();
        }
    }
    return nullptr;
}

} // namespace ecs
} // namespace bl

#endif
