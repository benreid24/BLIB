#ifndef BLIB_SYSTEMS_PHYSICS3D_HPP
#define BLIB_SYSTEMS_PHYSICS3D_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Components/Physics3D.hpp>
#include <BLIB/Engine/Events/Worlds.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <BLIB/Systems/Jolt/BroadphaseLayerInterface.hpp>
#include <BLIB/Systems/Jolt/JobSystem.hpp>
#include <BLIB/Systems/Jolt/ObjectLayerPairFilter.hpp>
#include <BLIB/Systems/Jolt/ObjectVsBroadphaseLayerFilter.hpp>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <optional>
#include <vector>

namespace bl
{
namespace sys
{
/**
 * @brief Engine system for 3d physics
 */
class Physics3D
: public engine::System
, public sig::Listener<engine::event::WorldCreated, engine::event::WorldDestroyed> {
public:
    /**
     * @brief Creates the system
     */
    Physics3D();

    /**
     * @brief Destroys the system
     */
    virtual ~Physics3D() = default;

    /**
     * @brief Returns the Jolt object layer pair filter. Used internally
     */
    JPH::ObjectLayerPairFilter& getObjectLayerPairFilter() { return objectLayerFilter; }

    /**
     * @brief Returns the Jolt broadphase layer interface. Used internally
     */
    JPH::BroadPhaseLayerInterface& getBroadphaseLayerInterface() {
        return broadphaseLayerInterface;
    }

    /**
     * @brief Returns the Jolt object vs broadphase layer filter. Used internally
     */
    JPH::ObjectVsBroadPhaseLayerFilter& getObjectLayerVsBroadphaseFilter() {
        return objectVsBroadphaseFilter;
    }

    /**
     * @brief Overrides the collision mask for the given object layer. Defaults are in
     *        com::Physics3D::ObjectCollisionMasks and do not usually need to be overridden
     *
     * @param layer The layer to set the mask for
     * @param mask The mask to use for the layer
     */
    void setObjectLayerCollisionMask(com::Physics3D::ObjectLayer layer, std::uint16_t mask);

    /**
     * @brief Adds or overwrites a new broadphase layer. Only needed for custom layers
     *
     * @param layer The broadphase layer to add or overwrite
     * @param name The name of the layer
     */
    void addBroadphaseLayer(com::Physics3D::BroadphaseLayer layer, const char* name);

    /**
     * @brief Sets the mapping from object layer to broadphase layer. Only needed for custom layers
     *
     * @param layerThe object layer to set the mapping for
     * @param broadphase The broadphase layer to map the object layer to
     */
    void setObjectLayerToBroadphaseMapping(com::Physics3D::ObjectLayer layer,
                                           com::Physics3D::BroadphaseLayer broadphase);

private:
    std::optional<jolt::JobSystem> jobSystem;
    JPH::TempAllocatorImpl joltFrameAllocator;
    jolt::ObjectLayerPairFilter objectLayerFilter;
    jolt::BroadPhaseLayerInterface broadphaseLayerInterface;
    jolt::ObjectVsBroadPhaseLayerFilter objectVsBroadphaseFilter;
    std::vector<JPH::PhysicsSystem*> activePhysicsSystems;

    virtual void init(engine::Engine& engine) override;
    virtual void earlyCleanup() override;
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;

    virtual void process(const engine::event::WorldCreated& worldCreate) override;
    virtual void process(const engine::event::WorldDestroyed& worldDestroy) override;
};

} // namespace sys
} // namespace bl

#endif
