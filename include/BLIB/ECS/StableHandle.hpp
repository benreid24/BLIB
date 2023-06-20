#ifndef BLIB_ECS_STABLEHANDLE_HPP
#define BLIB_ECS_STABLEHANDLE_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/Registry.hpp>

namespace bl
{
namespace ecs
{
class Registry;

/**
 * @brief Utility class to maintain a stable pointer to a component even if the pool is resizable
 *
 * @tparam T The component type to get a handle to
 * @ingroup ECS
 */
template<typename T>
class StableHandle {
public:
    /**
     * @brief Creates an empty handle
     */
    StableHandle();

    /**
     * @brief Creates a handle to the component of type T for the given entity
     *
     * @param registry The ECS registry
     * @param entity The entity to get the handle for
     */
    StableHandle(Registry& registry, Entity entity);

    /**
     * @brief Returns whether or not the handle can be used
     */
    bool valid() const;

    /**
     * @brief Returns the component the handle points to. Must be valid
     */
    T& get();

    /**
     * @brief Returns the component the handle points to. Must be valid
     */
    const T& get() const;

    /**
     * @brief Creates a handle to the component of type T for the given entity
     *
     * @param registry The ECS registry
     * @param entity The entity to get the handle for
     */
    void assign(Registry& registry, Entity entity);

    /**
     * @brief Invalidates the handle. Does not remove the component
     */
    void release();

private:
    ComponentPool<T>* pool;
    Entity entity;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
StableHandle<T>::StableHandle()
: pool(nullptr)
, entity(InvalidEntity) {}

template<typename T>
StableHandle<T>::StableHandle(Registry& registry, Entity entity) {
    assign(registry, entity);
}

template<typename T>
bool StableHandle<T>::valid() const {
    return pool && entity != InvalidEntity ? pool->get(entity) != nullptr : false;
}

template<typename T>
T& StableHandle<T>::get() {
#ifdef BLIB_DEBUG
    if (!valid()) { throw std::runtime_error("Dereferencing invalid ECS handle"); }
#endif
    return *pool->get(entity);
}

template<typename T>
const T& StableHandle<T>::get() const {
#ifdef BLIB_DEBUG
    if (!valid()) { throw std::runtime_error("Dereferencing invalid ECS handle"); }
#endif
    return *pool->get(entity);
}

template<typename T>
void StableHandle<T>::assign(Registry& registry, Entity ent) {
    pool   = &registry.getAllComponents<T>();
    entity = ent;
}

template<typename T>
void StableHandle<T>::release() {
    pool   = nullptr;
    entity = InvalidEntity;
}

} // namespace ecs
} // namespace bl

#endif
