#include <BLIB/Systems/Physics3D.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Systems/Jolt/Constants.hpp>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <cstdarg>

/*
  Integration TODO:
    X Interface for adding hitboxes and physics to entities
    X Entity layers
      X Mapping to broadphase layers (BPLayerInterfaceImpl)
      X Object layer filter impl (ObjectVsBroadPhaseLayerFilterImpl)
      X Object pair layer filter impl (ObjectLayerPairFilterImpl)
    - Signals for collisions
    - Signals for sensors
    X Custom job system on top of engine threadpool
    X Physics system update (instanced per world)
    - Can we automatically call OptimizeBroadPhase at certain points?
    X Writeback position & orientation (body activation listener to filter?)

  Reference: https://github.com/jrouwe/JoltPhysicsHelloWorld/blob/main/Source/HelloWorld.cpp
*/

namespace bl
{
namespace sys
{
namespace
{
constexpr unsigned int FrameMemory = 16 * 1024 * 1024; // 16 MB
constexpr float NominalDt          = 1.f / 60.f;       // 60 Hz

void joltLog(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[2048];
    std::vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    BL_LOG_INFO << "Jolt: " << buffer;
}
} // namespace

Physics3D::Physics3D()
: System(engine::FrameStage::Physics, engine::StateMask::Running)
, joltFrameAllocator(FrameMemory)
, physicsComponents(nullptr) {
    objectVsBroadphaseFilter.update(objectLayerFilter, broadphaseLayerInterface);
}

void Physics3D::init(engine::Engine& engine) {
    jobSystem.emplace(engine.engineLoopThreadpool());
    physicsComponents = &engine.ecs().getAllComponents<com::Physics3D>();

    JPH::Trace = &joltLog;
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
}

void Physics3D::earlyCleanup() {
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void Physics3D::update(std::mutex&, float dt, float, float, float) {
    const unsigned int multipleOf60FPS = std::ceil(dt / NominalDt);
    const unsigned int collisionSteps =
        std::max(multipleOf60FPS * jolt::Constants::CollisionStepsPer60FPS, 1u);
    for (JPH::PhysicsSystem* simulation : activePhysicsSystems) {
        if (!simulation) { continue; }
        simulation->Update(dt, collisionSteps, &joltFrameAllocator, &jobSystem.value());
    }

    // TODO - maybe we could reduce the scope of this loop via a list of active bodies?
    physicsComponents->forEach([](ecs::Entity, com::Physics3D& p) { p.writebackTransform(); });
}

void Physics3D::process(const engine::event::WorldCreated& worldCreate) {
    engine::World3D* world = dynamic_cast<engine::World3D*>(&worldCreate.world);
    if (world) {
        if (world->worldIndex() >= activePhysicsSystems.size()) {
            activePhysicsSystems.resize(world->worldIndex() + 1, nullptr);
        }
        activePhysicsSystems[world->worldIndex()] = &world->getPhysicsSystem();
    }
}

void Physics3D::process(const engine::event::WorldDestroyed& worldDestroy) {
    engine::World3D* world = dynamic_cast<engine::World3D*>(&worldDestroy.world);
    if (world) {
        if (world->worldIndex() >= activePhysicsSystems.size()) { return; }
        activePhysicsSystems[world->worldIndex()] = nullptr;
    }
}

void Physics3D::setObjectLayerCollisionMask(com::Physics3D::ObjectLayer layer, std::uint16_t mask) {
    objectLayerFilter.setObjectLayerCollisionMask(layer, mask);
    objectVsBroadphaseFilter.update(objectLayerFilter, broadphaseLayerInterface);
}

void Physics3D::addBroadphaseLayer(com::Physics3D::BroadphaseLayer layer, const char* name) {
    broadphaseLayerInterface.addBroadphaseLayer(layer, name);
}

void Physics3D::setObjectLayerToBroadphaseMapping(com::Physics3D::ObjectLayer layer,
                                                  com::Physics3D::BroadphaseLayer broadphase) {
    broadphaseLayerInterface.setObjectLayerBroadphase(layer, broadphase);
    objectVsBroadphaseFilter.update(objectLayerFilter, broadphaseLayerInterface);
}

} // namespace sys
} // namespace bl
