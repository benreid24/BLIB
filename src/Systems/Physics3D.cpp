#include <BLIB/Systems/Physics3D.hpp>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

/*
  Integration TODO:
    - Interface for adding hitboxes and physics to entities
    - Entity layers
      - Mapping to broadphase layers (BPLayerInterfaceImpl)
      - Object layer filter impl (ObjectVsBroadPhaseLayerFilterImpl)
      - Object pair layer filter impl (ObjectLayerPairFilterImpl)
    - Signals for collisions
    - Signals for sensors
    - World to physics scale?
    - Custom job system on top of engine threadpool
    - Physics system update (instanced per world)
    - Can we automatically call OptimizeBroadPhase at certain points?

  Reference: https://github.com/jrouwe/JoltPhysicsHelloWorld/blob/main/Source/HelloWorld.cpp
*/

namespace bl
{
namespace sys
{
namespace
{
constexpr unsigned int FrameMemory = 16 * 1024 * 1024; // 16 MB
}

Physics3D::Physics3D()
: System(engine::FrameStage::Physics, engine::StateMask::Running)
, joltFrameAllocator(FrameMemory) {}

void Physics3D::init(engine::Engine& engine) {
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
}

void Physics3D::update(std::mutex& stageMutex, float dt, float realDt, float residual,
                       float realResidual) {
    // TODO
}

} // namespace sys
} // namespace bl
