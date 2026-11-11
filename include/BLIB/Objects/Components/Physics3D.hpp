#ifndef BLIB_OBJECTS_COMPONENTS_PHYSICS3D_HPP
#define BLIB_OBJECTS_COMPONENTS_PHYSICS3D_HPP

#include <BLIB/Components/Physics3D.hpp>
#include <BLIB/ECS/EntityBacked.hpp>

namespace bl
{
namespace obj
{
namespace bcom
{
/**
 * @brief Base class for objects that may have 3d physics. Manages com::Physics3D
 *
 * @ingroup Objects
 */
class Physics3D : protected virtual ecs::EntityBacked {
public:
    /**
     * @brief Creates the physics component and body for this object
     *
     * @param motionType The motion type of the body
     * @param layer The object layer of the body
     * @param bodySettings The settings to use for the body
     * @param activation The activation state of the body
     */
    void createPhysicsBody(
        JPH::EMotionType motionType, com::Physics3D::ObjectLayer layer,
        const com::Physics3D::BodySettings& bodySettings = com::Physics3D::BodySettings(),
        JPH::EActivation activation                      = JPH::EActivation::Activate);

    /**
     * @brief Creates the physics component and body for this object
     *
     * @param shape The shape to use for the physics body
     * @param motionType The motion type of the body
     * @param layer The object layer of the body
     * @param bodySettings The settings to use for the body
     * @param activation The activation state of the body
     */
    void createPhysicsBody(
        JPH::Ref<JPH::Shape> shape, JPH::EMotionType motionType, com::Physics3D::ObjectLayer layer,
        const com::Physics3D::BodySettings& bodySettings = com::Physics3D::BodySettings(),
        JPH::EActivation activation                      = JPH::EActivation::Activate);

    /**
     * @brief Returns whether the physics body is currently added to the simulation
     */
    bool isAddedToPhysicsSimulation() const;

    /**
     * @brief Adds the physics body to the simulation if it is not already added
     *
     * @param activation The activation state of the body
     */
    void addToPhysicsSimulation(JPH::EActivation activation = JPH::EActivation::Activate);

    /**
     * @brief Removes the physics body from the simulation if it is currently added
     */
    void removeFromPhysicsSimulation();

    /**
     * @brief Manually sets the linear velocity of the physics body
     *
     * @param velocity The new linear velocity to set for the body
     */
    void setLinearVelocity(const glm::vec3& velocity);

    /**
     * @brief Manually sets the position of the physics body
     *
     * @param position The new position to set for the body
     * @param activation The activation state of the body
     */
    void setPosition(const glm::vec3& position,
                     JPH::EActivation activation = JPH::EActivation::Activate);

    /**
     * @brief Returns a direct handle to the physics component
     */
    com::Physics3D* getPhysicsComponent() { return physics; }

protected:
    /**
     * @brief Initializes the state to empty
     */
    Physics3D();

    /**
     * @brief Must be implemented by derived classes to define their shape
     *
     * @param material The physics material to use for the shape
     * @return A unique pointer to the created shape
     */
    virtual JPH::Ref<JPH::Shape> createPhysicsShape(JPH::Ref<JPH::PhysicsMaterial> material) = 0;

private:
    com::Physics3D* physics;
};

} // namespace bcom
} // namespace obj
} // namespace bl

#endif
