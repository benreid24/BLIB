#ifndef BLIB_COMPONENTS_PHYSICS2D_HPP
#define BLIB_COMPONENTS_PHYSICS2D_HPP

#include <BLIB/Components/Hitbox2D.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/ECS/Entity.hpp>
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
     * @param system The physics system
     * @param entity The owning entity of this component
     * @param transform The transform of the owning entity
     * @param bodyId The body id from Box2D
     */
    Physics2D(sys::Physics2D& system, ecs::Entity owner, Transform2D& transform, b2BodyId bodyId);

    /**
     * @brief Applies a force, in Newtons, to the physics body CoM
     *
     * @param force The force in Newtons
     */
    void applyForceToCenter(const glm::vec2& force);

    /**
     * @brief Applies an impulse, in kg*m/s, to the physics body CoM
     *
     * @param impulse The impulse in kg*m/s
     */
    void applyImpulseToCenter(const glm::vec2& impulse);

    /**
     * @brief Teleports the entity to the given position. Setting the position on the Transform2D
     *        component will just get overwritten, this method must be used instead when physics is
     *        active on an entity
     *
     * @param position The position to teleport to
     * @param rotation The new rotation in degrees
     * @param cancelVelocity Whether to set the velocity to 0 after teleport or not
     */
    void teleport(const glm::vec2& position, float rotation = 0.f, bool cancelVelocity = true);

private:
    sys::Physics2D* system;
    ecs::Entity entity;
    b2BodyId bodyId;
    Transform2D* transform;

    friend class sys::Physics2D;
};

} // namespace com
} // namespace bl

#endif
