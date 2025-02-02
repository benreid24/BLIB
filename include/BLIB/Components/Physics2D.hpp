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
     * @brief Returns the entity id that owns this component
     */
    ecs::Entity getOwner() const { return entity; }

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

    /**
     * @brief Sets the velocity of the object
     *
     * @param velocity The new velocity
     */
    void setVelocity(const glm::vec2& velocity);

    /**
     * @brief Sets the linear velocity damping factor
     *
     * @param damping The linear damping factor
     */
    void setLinearDamping(float damping);

    /**
     * @brief Returns the transform for the entity
     */
    Transform2D& getTransform() { return *transform; }

    /**
     * @brief Returns the transform for the entity
     */
    const Transform2D& getTransform() const { return *transform; }

    /**
     * @brief Returns the mass of this physics object
     */
    float getMass() const;

    /**
     * @brief Scales the given world distance value to a physics system distance value
     *
     * @param worldDistance The distance value in world units
     * @return The distance value in physics units
     */
    float scaleWorldToPhysics(float worldDistance);

    /**
     * @brief Scales the given physics distance value to world distance
     *
     * @param worldDistance The distance value in physics units
     * @return The distance value in world units
     */
    float scalePhysicToWorld(float physics);

    /**
     * @brief Limits the linear velocity to the max velocity. Must call continuously to keep
     *        limiting each frame
     *
     * @param maxVelocity The maximum linear velocity, in world units
     */
    void clampLinearVelocity(float maxVelocity);

    /**
     * @brief Returns the linear velocity in physics units
     */
    glm::vec2 getLinearVelocity() const;

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
