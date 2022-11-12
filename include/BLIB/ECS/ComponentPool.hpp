#ifndef BLIB_ECS_COMPONENTPOOL_HPP
#define BLIB_ECS_COMPONENTPOOL_HPP

#include <BLIB/Containers/RingQueue.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <limits>
#include <vector>

namespace bl
{
namespace ecs
{
class Registry;

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
     * @brief Removes the component from the given entity
     *
     * @param entity The entity to remove the component from
     */
    virtual void remove(Entity entity) = 0;

    /**
     * @brief Destroys all components
     *
     */
    virtual void clear() = 0;

    /**
     * @brief Returns the number of different component types that exist
     *
     * @return unsigned int The number of unique component types
     */
    static unsigned int ComponentCount();

protected:
    ComponentPoolBase()
    : ComponentIndex(nextComponentIndex++) {}

    static unsigned int nextComponentIndex;
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
     * @brief Special iterator for iterating over a single component type in the ECS
     *
     */
    class Iterator {
    public:
        Iterator(const Iterator&) = default;
        Iterator(Iterator&&)      = default;
        Iterator& operator=(const Iterator&) = default;
        Iterator& operator=(Iterator&&) = default;

        T& operator*();
        const T& operator*() const;
        T& operator->();
        const T& operator->() const;
        Iterator& operator++();
        Iterator& operator++(int);
        bool operator==(const Iterator& it) const;
        bool operator!=(const Iterator& it) const;
        Entity entity() const;

    private:
        std::vector<container::ObjectWrapper<T>>::iterator it;
        ComponentPool* pool;
    };

    /**
     * @brief Fetches the component for the given entity if it exists
     *
     * @param entity The entity to get the component for
     * @return T* Pointer to the component or nullptr if the entity does not have one
     */
    T* get(Entity entity);

    /**
     * @brief Returns an iterator to the first component in the pool
     *
     */
    Iterator begin();

    /**
     * @brief Returns an iterator to the last component in the pool
     *
     */
    Iterator end();

private:
    std::vector<container::ObjectWrapper<T>> pool;
    std::vector<std::uint16_t> entityToIndex;
    std::vector<Entity> indexToEntity;
    std::vector<bool> aliveIndices;
    container::RingQueue<std::uint16_t> freeIndexes;
    std::uint16_t nextIndex;

    ComponentPool(std::size_t poolSize);
    static ComponentPool& get(std::size_t poolSize);

    T* add(Entity entity, const T& component);
    virtual void remove(Entity entity) override;
    virtual void clear() override;

    friend class Registry;
};

} // namespace ecs
} // namespace bl

#endif
