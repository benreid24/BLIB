#ifndef BLIB_ENTITIES_COMPONENTSET_HPP
#define BLIB_ENTITIES_COMPONENTSET_HPP

#include <BLIB/Entities/Entity.hpp>

namespace bl
{
namespace entity
{
class Registry;

template<typename TComponent>
struct ComponentSetMember {
    TComponent* component;

    ComponentSetMember();
};

template<typename... TComponents>
class ComponentSet : private ComponentSetMember<TComponents>... {
public:
    ComponentSet(Entity owner);

    Entity getOwner() const;

    template<typename TComponent>
    TComponent* get();

    template<typename TComponent>
    const TComponent* get() const;

private:
    const Entity owner;

    template<typename TComponent>
    void set(TComponent* c);

    friend class Registry;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TComponent>
ComponentSetMember<TComponent>::ComponentSetMember()
: component(nullptr) {}

template<typename... TComponents>
ComponentSet<TComponents...>::ComponentSet(Entity owner)
: owner(owner) {}

template<typename... TComponents>
Entity ComponentSet<TComponents...>::getOwner() const {
    return owner;
}

template<typename... TComponents>
template<typename TComponent>
TComponent* ComponentSet<TComponents...>::get() {
    return ComponentSetMember<TComponent>::component;
}

template<typename... TComponents>
template<typename TComponent>
const TComponent* ComponentSet<TComponents...>::get() const {
    return ComponentSetMember<TComponent>::component;
}

template<typename... TComponents>
template<typename TComponent>
void ComponentSet<TComponents...>::set(TComponent* c) {
    ComponentSetMember<TComponent>::component = c;
}

} // namespace entity
} // namespace bl

#endif
