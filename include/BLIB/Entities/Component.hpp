#ifndef BLIB_ENTITIES_COMPONENT_HPP
#define BLIB_ENTITIES_COMPONENT_HPP

#include <BLIB/Entities/Entity.hpp>
#include <cstddef>

namespace bl
{
namespace entity
{
/**
 * @brief Helper class for components in the entity component system. Component classes must define
 *        a unique numeric id in the manner below:
 *
 *     static constexpr Component::IdType ComponentId = <unique-id>;
 *
 * @ingroup Entities
 *
 */
class Component {
public:
    using IdType = std::size_t;

    /**
     * @brief Returns `TComponent::ComponentId` which must be defined
     *
     */
    template<typename TComponent>
    static IdType getId();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TComponent>
Component::IdType Component::getId() {
    return TComponent::ComponentId;
}

} // namespace entity
} // namespace bl

#endif
