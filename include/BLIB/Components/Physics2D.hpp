#ifndef BLIB_COMPONENTS_PHYSICS2D_HPP
#define BLIB_COMPONENTS_PHYSICS2D_HPP

#include <BLIB/Components/Hitbox2D.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <box2d/box2d.h>

namespace bl
{
namespace sys
{
class Physics2D;
}

namespace com
{
/**
 * @brief Component that can be added to entities to give them 2d physics
 *
 * @ingroup Components
 */
class Physics2D {
public:
    /**
     * @brief Creates the physics component. Should not be used directly, use the helper in
     *        sys::Physics2D to add physics simulation to an entity
     *
     * @param transform The transform of the owning entity
     * @param bodyId The body id from Box2D
     */
    Physics2D(Transform2D& transform, b2BodyId bodyId);

private:
    b2BodyId bodyId;
    Transform2D* transform;

    friend class sys::Physics2D;
};

} // namespace com
} // namespace bl

#endif
