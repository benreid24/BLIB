#ifndef BLIB_ECS_PARENTAWARE_HPP
#define BLIB_ECS_PARENTAWARE_HPP

#include <type_traits>

namespace bl
{
namespace ecs
{
template<typename T>
class ComponentPool;

namespace trait
{

/**
 * @brief Component trait for components that are parent-able. Parent is set in the Registry as an
 *        entity-level relationship. Components should inherit this trait and pass their own type
 *        as the type parameter T. This trait provides a pointer to the parent component
 *
 * @tparam T The component type
 * @ingroup ECS
 */
template<typename T>
class ParentAware {
public:
    /**
     * @brief Sets the parent to nullptr
     */
    ParentAware();

    /**
     * @brief Returns whether or not a parent has been set
     */
    bool hasParent() const;

    /**
     * @brief Returns the parent component
     */
    T& getParent();

    /**
     * @brief Returns the parent component
     */
    const T& getParent() const;

private:
    T* parent;

    friend class ComponentPool<T>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ParentAware<T>::ParentAware()
: parent(nullptr) {}

template<typename T>
bool ParentAware<T>::hasParent() const {
    return parent != nullptr;
}

template<typename T>
T& ParentAware<T>::getParent() {
    return *parent;
}

template<typename T>
const T& ParentAware<T>::getParent() const {
    return *parent;
}

} // namespace trait
} // namespace ecs
} // namespace bl

#endif
