#ifndef BLIB_ENGINE_WORLDS_WORLD3D_HPP
#define BLIB_ENGINE_WORLDS_WORLD3D_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Engine/Worlds/BasicWorld.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>
#include <BLIB/Systems/Jolt/ContactListener.hpp>
#include <Jolt/Physics/PhysicsSystem.h>

namespace bl
{
namespace engine
{
/**
 * @brief World type for 3d worlds with physics and collisions
 *
 * @ingroup Engine
 */
class World3D : public BasicWorld<rc::scene::Scene3D> {
public:
    /**
     * @brief Creates the world
     *
     * @param owner The game engine instance
     */
    World3D(Engine& owner);

    /**
     * @brief Destroys the world
     */
    virtual ~World3D();

    /**
     * @brief Returns the Jolt physics system for this world
     */
    JPH::PhysicsSystem& getPhysicsSystem() { return physicsSystem; }

    /**
     * @brief Queues a physics body to be added to the simulation at the start of the next frame
     *
     * @param bodyId The body to add
     * @param activation The activation state of the body
     */
    void queueAddPhysicsBody(JPH::BodyID bodyId, JPH::EActivation activation);

    /**
     * @brief Queues a physics body to be removed from the simulation at the start of the next frame
     *
     * @param bodyId The body to remove
     */
    void queueRemovePhysicsBody(JPH::BodyID bodyId);

    /**
     * @brief Queues a physics body to be destroyed at the start of the next frame
     *
     * @param bodyId The body to destroy
     */
    void queueDestroyPhysicsBody(JPH::BodyID bodyId);

    /**
     * @brief Called by the physics system each frame
     */
    void drainPhysicsQueues();

private:
    JPH::PhysicsSystem physicsSystem;
    sys::jolt::ContactListener contactListener;
    std::vector<JPH::BodyID> bodiesToAddAndActivate;
    std::vector<JPH::BodyID> bodiesToAddNoActivate;
    std::vector<JPH::BodyID> bodiesToRemove;
    std::vector<JPH::BodyID> bodiesToDestroy;
};

} // namespace engine
} // namespace bl

#endif
