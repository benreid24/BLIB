#ifndef BLIB_ECS_COMPONENTSET_HPP
#define BLIB_ECS_COMPONENTSET_HPP

#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace ecs
{
class Registry;

namespace priv
{
/**
 * @brief Helper class to compose component sets
 *
 * @tparam T The single component type to make a member
 * @ingroup ECS
 */
template<typename T>
struct ComponentSetMember {
    T* component;
    ComponentSetMember();
    T* populate(Registry& registry, Entity owner);
};
} // namespace priv

/**
 * @brief Contains a set of components for a single entity. May not always be valid. Does not update
 *        if components are added or removed from the entity. Sets from views are always valid
 *
 * @tparam TComponents The components to fetch for the entity
 * @ingroup ECS
 */
template<typename... TComponents>
class ComponentSet : private priv::ComponentSetMember<TComponents>... {
public:
    /**
     * @brief Creates the set and fetches each component type
     *
     * @param registry The registry to get components from
     * @param owner The entity to get components for
     */
    ComponentSet(Registry& registry, Entity owner);

    /**
     * @brief Refreshes the components in the set. Call when pools resize
     *
     * @param registry The ECS registry
     * @return True if the set is still valid, false otherwise
     */
    bool refresh(Registry& registry);

    /**
     * @brief Returns the entity that the components belong to
     *
     * @return constexpr Entity The entity that the components belong to
     */
    constexpr Entity entity() const;

    /**
     * @brief Access the single component in the set
     *
     * @tparam T The component type to get
     * @return T* Pointer to the given component. May be nullptr
     */
    template<typename T>
    constexpr T* get();

    /**
     * @brief Access the single component in the set
     *
     * @tparam T The component type to get
     * @return const T* Pointer to the given component. May be nullptr
     */
    template<typename T>
    constexpr const T* get() const;

    /**
     * @brief Returns whether or not all components are present and non-null
     *
     * @return True if all components were found, false if some were not
     */
    constexpr bool isValid() const;

private:
    Entity owner;
    bool valid;
};

} // namespace ecs
} // namespace bl

#endif
