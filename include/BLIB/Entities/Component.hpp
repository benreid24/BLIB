#ifndef BLIB_ENTITIES_COMPONENT_HPP
#define BLIB_ENTITIES_COMPONENT_HPP

#include <BLIB/Entities/Entity.hpp>
#include <cstddef>

namespace bl
{
namespace entity
{
class Component {
public:
    using IdType = std::size_t;

    template<typename TComponent>
    static IdType getId();
};

template<typename TComponent>
class EntityComponent {
public:
    const Entity& entity;
    TComponent* component;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TComponent>
Component::IdType Component::getId() {
    return TComponent::ComponentId;
}

} // namespace entity
} // namespace bl

#endif
