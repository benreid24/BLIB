#ifndef BLIB_ECS_COMPONENTSETIMPL_HPP
#define BLIB_ECS_COMPONENTSETIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

namespace bl
{
namespace ecs
{
template<typename T>
ComponentSetMember<T>::ComponentSetMember(Registry& registry, Entity owner)
: component(registry.getComponent<T>(owner)) {}

template<typename... TComponents>
ComponentSet<TComponents...>::ComponentSet(Registry& registry, Entity ent)
: ComponentSetMember<TComponents>(registry, ent)...
, owner(ent)
, valid(true) {
    void* clist[] = {static_cast<void*>(get<TComponents>())...};
    for (unsigned int i = 0; i < std::size(clist); ++i) {
        if (clist[i] == nullptr) {
            valid = false;
            break;
        }
    }
}

template<typename... TComponents>
constexpr Entity ComponentSet<TComponents...>::entity() const {
    return owner;
}

template<typename... TComponents>
template<typename T>
constexpr T* ComponentSet<TComponents...>::get() {
    return ComponentSetMember<T>::component;
}

template<typename... TComponents>
template<typename T>
constexpr const T* ComponentSet<TComponents...>::get() const {
    return ComponentSetMember<T>::component;
}

template<typename... TComponents>
constexpr bool ComponentSet<TComponents...>::isValid() const {
    return valid;
}

} // namespace ecs
} // namespace bl

#endif
