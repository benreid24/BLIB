#ifndef BLIB_ECS_COMPONENTSETIMPL_HPP
#define BLIB_ECS_COMPONENTSETIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

#include <BLIB/ECS/ComponentSet.hpp>
#include <BLIB/ECS/Tags.hpp>

namespace bl
{
namespace ecs
{
namespace priv
{
template<typename T>
ComponentSetMember<T>::ComponentSetMember()
: component(nullptr) {}

template<typename T>
bool ComponentSetMember<T>::populate(Registry& registry, Entity owner,
                                     const txp::TransactionComponentRead<T>& tx) {
    component = registry.getAllComponents<T>().get(owner, tx);
    return component != nullptr;
}

template<typename T>
void ComponentSetMember<T>::nullout(void* com) {
    if (static_cast<void*>(component) == com) { component = nullptr; }
}

} // namespace priv

/**
 * @brief The actual ComponentSet specialization that should be used
 *
 * @tparam ...TReqComs The set of required components
 * @tparam ...TOptComs The set of optional components
 * @ingroup ECS
 */
template<typename... TReqComs, typename... TOptComs>
class ComponentSet<Require<TReqComs...>, Optional<TOptComs...>>
: private priv::ComponentSetMember<TReqComs>...
, private priv::ComponentSetMember<TOptComs>... {
public:
    /**
     * @brief Creates the set and fetches each component type
     *
     * @param registry The registry to get components from
     * @param owner The entity to get components for
     */
    ComponentSet(Registry& registry, Entity owner)
    : owner(owner)
    , valid(true) {
        refresh(registry);
    }

    /**
     * @brief Creates the set and fetches each component type
     *
     * @param registry The registry to get components from
     * @param owner The entity to get components for
     * @param transaction The transaction to use to synchronize access
     */
    ComponentSet(Registry& registry, Entity owner,
                 const Transaction<tx::EntityUnlocked, tx::ComponentRead<TReqComs..., TOptComs...>>&
                     transaction)
    : owner(owner)
    , valid(true) {
        refresh(registry, transaction);
    }

    /**
     * @brief Refreshes the components in the set. Call when pools resize
     *
     * @param registry The ECS registry
     * @return True if the set is still valid, false otherwise
     */
    bool refresh(Registry& registry) {
        return refresh(
            registry,
            Transaction<tx::EntityUnlocked, tx::ComponentRead<TReqComs..., TOptComs...>>(registry));
    }

    /**
     * @brief Refreshes the components in the set. Call when pools resize
     *
     * @param registry The ECS registry
     * @param transaction The transaction to use to synchronize access
     * @return True if the set is still valid, false otherwise
     */
    bool refresh(Registry& registry,
                 const Transaction<tx::EntityUnlocked, tx::ComponentRead<TReqComs..., TOptComs...>>&
                     transaction) {
        valid = (priv::ComponentSetMember<TReqComs>::populate(registry, owner, transaction) && ...);
        if constexpr (sizeof...(TOptComs) > 0) {
            (priv::ComponentSetMember<TOptComs>::populate(registry, owner, transaction), ...);
        }

        return valid;
    }

    /**
     * @brief Returns the entity that the components belong to
     *
     * @return constexpr Entity The entity that the components belong to
     */
    constexpr Entity entity() const { return owner; }

    /**
     * @brief Access the single component in the set
     *
     * @tparam T The component type to get
     * @return T* Pointer to the given component. May be nullptr
     */
    template<typename T>
    constexpr T* get() {
        return priv::ComponentSetMember<T>::component;
    }

    /**
     * @brief Access the single component in the set
     *
     * @tparam T The component type to get
     * @return const T* Pointer to the given component. May be nullptr
     */
    template<typename T>
    constexpr const T* get() const {
        return priv::ComponentSetMember<T>::component;
    }

    /**
     * @brief Returns whether or not all components are present and non-null
     *
     * @return True if all components were found, false if some were not
     */
    constexpr bool isValid() const { return valid; }

    /**
     * @brief Helper method to null a component in the component set. Intended to be used internally
     *
     * @param com Pointer to the component to null
     */
    void removeComponent(void* com) {
        if constexpr (sizeof...(TOptComs) > 0) {
            (void)std::initializer_list<int>{
                (priv::ComponentSetMember<TOptComs>::nullout(com), 0)...};
        }
    }

private:
    Entity owner;
    bool valid;
};

} // namespace ecs
} // namespace bl

#endif
