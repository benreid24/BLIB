#ifndef BLIB_ENTITIES_COMPONENTSET_HPP
#define BLIB_ENTITIES_COMPONENTSET_HPP

#include <BLIB/Entities/Entity.hpp>

namespace bl
{
namespace entity
{
class Registry;

/**
 * @brief Helper base class for tuple like functionality
 *
 * @ingroup Entities
 */
template<typename TComponent>
struct ComponentSetMember {
    TComponent* component;

    ComponentSetMember();
};

/**
 * @brief Tuple like class that stores a set of components by pointer value
 *
 * @tparam TComponents The set of components contained
 * @ingroup Entities
 */
template<typename... TComponents>
class ComponentSet : private ComponentSetMember<TComponents>... {
public:
    /**
     * @brief Create the ComponentSet with the given owner
     *
     * @param owner The Entity that owns the stored components
     */
    ComponentSet(Entity owner);

    /**
     * @brief Returns the owner of the components in the set
     *
     */
    Entity getOwner() const;

    /**
     * @brief Returns a pointer to the component of the given type. May be nullptr
     *
     * @tparam TComponent The type of component to access. Must be contained in set
     * @return TComponent* Pointer to the component. May be nullptr if entity does not have it
     */
    template<typename TComponent>
    TComponent* get();

    /**
     * @brief Returns a pointer to the component of the given type. May be nullptr
     *
     * @tparam TComponent The type of component to access. Must be contained in set
     * @return TComponent* Pointer to the component. May be nullptr if entity does not have it
     */
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
