#ifndef BLIB_COMPONENTS_PHYSICS3DCHARACTER_HPP
#define BLIB_COMPONENTS_PHYSICS3DCHARACTER_HPP

#include <Jolt/Jolt.h>

#include <BLIB/ECS/Entity.hpp>
#include <Jolt/Physics/Character/Character.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace bl
{
namespace engine
{
class World3D;
}
namespace sys
{
class Physics3D;
}

namespace com
{
class Transform3D;

/**
 * @brief Represents a controllable character with simulated physics. Built on top of JPH::Character
 *
 * @ingroup Components
 */
class Physics3DCharacter {
public:
    static constexpr float MaxSeparationDistance = 0.1f;

    /**
     * @brief Creates the character component
     *
     * @param entity The entity to attach the component to
     * @param world The world the entity is in
     * @param settings The settings for the character
     */
    Physics3DCharacter(ecs::Entity entity, engine::World3D& world,
                       const JPH::CharacterSettings& settings);

    /**
     * @brief Adds the character to the physics simulation
     *
     * @param activation Whether to activate the body immediately
     */
    void addToSimulation(JPH::EActivation activation = JPH::EActivation::Activate);

    /**
     * @brief Removes the character from the physics simulation
     */
    void removeFromSimulation();

    /**
     * @brief Sets the linear velocity of the character
     *
     * @param velocity The linear velocity to set
     */
    void setLinearVelocity(const glm::vec3& velocity);

    /**
     * @brief Adds the velocity to the current linear velocity
     *
     * @param velocity The change in velocity to apply
     */
    void addLinearVelocity(const glm::vec3& velocity);

    /**
     * @brief Applies an impulse to the character
     *
     * @param impulse The impulse to apply
     */
    void addImpulse(const glm::vec3& impulse);

    /**
     * @brief Sets the position of the character
     *
     * @param position The position in world units
     */
    void setPosition(const glm::vec3& position);

    /**
     * @brief Sets the rotation of the character
     *
     * @param rotation The rotation to set
     */
    void setRotation(const glm::quat& rotation);

    /**
     * @brief Sets the position and rotation of the character
     *
     * @param position The position in world units
     * @param rotation The rotation to set
     */
    void setPositionAndRotation(const glm::vec3& position,
                                const glm::quat& rotation = glm::quat(1.f, 0.f, 0.f, 0.f));

    /**
     * @brief Sets the shape of the character
     *
     * @param shape The physics shape
     * @param maxPenetrationDepth The maximum allowed penetration depth
     * @return True if the shape was set, false if the max penetration depth was not satisfied
     */
    bool setShape(JPH::Ref<JPH::Shape> shape, float maxPenetrationDepth = MaxSeparationDistance);

private:
    JPH::CharacterSettings settings;
    JPH::Character character;
    com::Transform3D* transform;
    float maxSeparationDistance;
    bool isAdded;

    Physics3DCharacter(ecs::Entity entity, engine::World3D& world, Transform3D* transform,
                       const JPH::CharacterSettings& settings);

    void postUpdate();

    friend class sys::Physics3D;
};

} // namespace com
} // namespace bl

#endif
