#ifndef BLIB_ECS_COMPONENTSETIMPL_HPP
#define BLIB_ECS_COMPONENTSETIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

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
T* ComponentSetMember<T>::populate(Registry& registry, Entity owner) {
    component = registry.getComponent<T>(owner);
    return component;
}

} // namespace priv

template<typename... TComponents>
ComponentSet<TComponents...>::ComponentSet(Registry& registry, Entity ent)
: priv::ComponentSetMember<TComponents>()...
, owner(ent)
, valid(true) {
    refresh(registry);
}

template<typename... TComponents>
bool ComponentSet<TComponents...>::refresh(Registry& registry) {
    valid         = true;
    void* clist[] = {
        static_cast<void*>(priv::ComponentSetMember<TComponents>::populate(registry, owner))...};
    for (unsigned int i = 0; i < std::size(clist); ++i) {
        if (clist[i] == nullptr) { valid = false; }
    }
    return valid;
}

template<typename... TComponents>
constexpr Entity ComponentSet<TComponents...>::entity() const {
    return owner;
}

template<typename... TComponents>
template<typename T>
constexpr T* ComponentSet<TComponents...>::get() {
    return priv::ComponentSetMember<T>::component;
}

template<typename... TComponents>
template<typename T>
constexpr const T* ComponentSet<TComponents...>::get() const {
    return priv::ComponentSetMember<T>::component;
}

template<typename... TComponents>
constexpr bool ComponentSet<TComponents...>::isValid() const {
    return valid;
}

} // namespace ecs
} // namespace bl

#endif
