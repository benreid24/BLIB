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

    Component(Entity owner);

    virtual ~Component() = default;

    Entity getOwner() const;

private:
    const Entity owner;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline Component::Component(Entity e)
: owner(e) {}

inline Entity Component::getOwner() const { return owner; }

} // namespace entity
} // namespace bl

#endif
