#ifndef BLIB_COMPONENTS_PHYSICS3D_HPP
#define BLIB_COMPONENTS_PHYSICS3D_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Components/Transform3D.hpp>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/EActivation.h>
#include <limits>

namespace bl
{
namespace sys
{
class Physics3D;
}

namespace com
{
/**
 * @brief Component that adds 3d physics simulation to an entity
 *
 * @ingroup Components
 */
class Physics3D {
public:
    /**
     * @brief The layer the entity is in. Can be customized via sys::Physics3D if necessary. Values
     *        should be in range [0, 16)
     */
    enum struct ObjectLayer : JPH::ObjectLayer {
        Terrain = 0,
        Dynamic = 1,
        Bullet  = 2,
        Debris  = 3,
        Count   = 4
    };

    /// The maximum number of distinct object layers
    static constexpr std::size_t MaxObjectLayers = std::numeric_limits<JPH::ObjectLayer>::digits;

    /**
     * @brief The broadphase layer the object is in. Can be customized via sys::Physics3D if
     *        necessary. Values should be in the range [0, 8)
     */
    enum struct BroadphaseLayer : JPH::BroadPhaseLayer::Type {
        Moving    = 0,
        NonMoving = 1,
        Count     = 2
    };

    /// The maximum number of distinct broadphase layers
    static constexpr std::size_t MaxBroadphaseLayers =
        std::numeric_limits<JPH::BroadPhaseLayer::Type>::digits;

    /**
     * @brief Helper containing bitmasks for the default object layers
     */
    struct Bitmasks {
        static constexpr std::uint16_t TerrainBitmask = 0x1 << int(ObjectLayer::Terrain);
        static constexpr std::uint16_t DynamicBitmask = 0x1 << int(ObjectLayer::Dynamic);
        static constexpr std::uint16_t BulletBitmask  = 0x1 << int(ObjectLayer::Bullet);
        static constexpr std::uint16_t DebrisBitmask  = 0x1 << int(ObjectLayer::Debris);
        static constexpr std::uint16_t All            = 0xFFFF;
        static constexpr std::uint16_t None           = 0;

        static constexpr std::uint16_t Masks[] = {
            TerrainBitmask,
            DynamicBitmask,
            BulletBitmask,
            DebrisBitmask,
        };
    };

    /**
     * @brief Contains the default interation rules for the builtin set of object layers
     */
    struct ObjectCollisionMasks {
        static constexpr std::uint16_t TerrainMask = Bitmasks::None;
        static constexpr std::uint16_t DynamicMask =
            Bitmasks::TerrainBitmask | Bitmasks::DynamicBitmask;
        static constexpr std::uint16_t BulletMask =
            Bitmasks::TerrainBitmask | Bitmasks::DynamicBitmask;
        static constexpr std::uint16_t DebrisMask =
            Bitmasks::TerrainBitmask | Bitmasks::DynamicBitmask;

        static constexpr std::uint16_t Masks[] = {
            TerrainMask,
            DynamicMask,
            BulletMask,
            DebrisMask,
        };
    };

    /**
     * @brief Contains the mapping from object layer to broadphase layer for the default set
     */
    struct BroadphaseObjectMapping {
        static constexpr BroadphaseLayer Mapping[] = {
            BroadphaseLayer::Moving,    // Terrain
            BroadphaseLayer::Moving,    // Dynamic
            BroadphaseLayer::Moving,    // Bullet
            BroadphaseLayer::NonMoving, // Debris
        };
    };

    /**
     * @brief Adds physics to the entity and adds it to the simulation
     *
     * @param engine The engine instance
     * @param entity The entity to add physics to
     * @param shape The physics shape to use for the entity
     * @param motionType The motion type of the entity
     * @param layer The object layer of the entity
     * @param activation The activation state of the entity
     */
    Physics3D(engine::Engine& engine, ecs::Entity entity, std::unique_ptr<JPH::Shape>&& shape,
              JPH::EMotionType motionType, ObjectLayer layer, JPH::EActivation activation);

    /**
     * @brief Adds physics to the entity but does not add it to the simulation
     *
     * @param engine The engine instance
     * @param entity The entity to add physics to
     * @param shape The physics shape to use for the entity
     * @param motionType The motion type of the entity
     * @param layer The object layer of the entity
     */
    Physics3D(engine::Engine& engine, ecs::Entity entity, std::unique_ptr<JPH::Shape>&& shape,
              JPH::EMotionType motionType, ObjectLayer layer);

    /**
     * @brief Returns whether the entity is currently added to the simulation
     */
    bool isAddedToSimulation() const { return isAdded; }

    /**
     * @brief Adds the entity to the simulation if it is not already added
     *
     * @param activation The activation state of the entity
     */
    void addToSimulation(JPH::EActivation activation = JPH::EActivation::Activate);

    /**
     * @brief Removes the entity from the simulation if it is currently added
     */
    void removeFromSimulation();

    /**
     * @brief Manually sets the linear velocity of the entity
     *
     * @param velocity The new linear velocity to set for the entity
     */
    void setLinearVelocity(const glm::vec3& velocity);

    /**
     * @brief Manually sets the position of the entity
     *
     * @param position The new position to set for the entity
     * @param activation The activation state of the entity
     */
    void setPosition(const glm::vec3& position,
                     JPH::EActivation activation = JPH::EActivation::Activate);

    /**
     * @brief Pulls the position and rotation from the simulation and writes it to the ECS. Does not
     *        need to be manually called
     */
    void writebackTransform();

private:
    ecs::Entity entity;
    JPH::BodyInterface* bodyInterface;
    std::unique_ptr<JPH::Shape> shape;
    com::Transform3D* transform;
    JPH::Body* body;
    bool isAdded;
};

} // namespace com
} // namespace bl

#endif
