#ifndef BLIB_ECS_CHILDAWARE_HPP
#define BLIB_ECS_CHILDAWARE_HPP

#include <type_traits>
#include <vector>

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
 *        as the type parameter T. This trait provides a vector of child pointers on the component
 *
 * @tparam T The component type
 * @ingroup ECS
 */
template<typename T>
class ChildAware {
public:
    /**
     * @brief Sets the parent to nullptr
     */
    ChildAware();

    /**
     * @brief Returns the list of child components
     */
    const std::vector<T*>& getChildren() const;

private:
    std::vector<T*> children;

    friend class ComponentPool<T>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ChildAware<T>::ChildAware() {}

template<typename T>
inline const std::vector<T*>& ChildAware<T>::getChildren() const {
    return children;
}

} // namespace trait
} // namespace ecs
} // namespace bl

#endif
