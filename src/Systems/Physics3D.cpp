#include <BLIB/Systems/Physics3D.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Systems/Jolt/Constants.hpp>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <cstdarg>

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
, physicsComponents(nullptr) {
    objectVsBroadphaseFilter.update(objectLayerFilter, broadphaseLayerInterface);
    activePhysicsSystems.reserve(4);
    firstTickWorlds.reserve(4);
}

void Physics3D::init(engine::Engine& engine) {
    signalChannel.setParent(engine.getSignalChannel());

    JPH::RegisterDefaultAllocator();
    joltFrameAllocator.emplace(FrameMemory);

    jobSystem.emplace(engine.engineLoopThreadpool());
    physicsComponents   = &engine.ecs().getAllComponents<com::Physics3D>();
    characterComponents = &engine.ecs().getAllComponents<com::Physics3DCharacter>();

    JPH::Trace = &joltLog;
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    subscribe(engine.getSignalChannel());
}

void Physics3D::earlyCleanup() {
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void Physics3D::update(std::mutex&, float dt, float, float, float) {
    for (engine::World3D* world : activePhysicsSystems) {
        if (world) { world->drainPhysicsQueues(); }
    }

    for (engine::World3D* world : firstTickWorlds) {
        world->getPhysicsSystem().OptimizeBroadPhase();
    }
    firstTickWorlds.clear();

    const unsigned int multipleOf60FPS = std::ceil(dt / NominalDt);
    const unsigned int collisionSteps =
        std::max(multipleOf60FPS * jolt::Constants::CollisionStepsPer60FPS, 1u);
    for (engine::World3D* world : activePhysicsSystems) {
        if (!world) { continue; }
        world->getPhysicsSystem().Update(
            dt, collisionSteps, &*joltFrameAllocator, &jobSystem.value());
    }

    physicsComponents->forEach([dt](ecs::Entity, com::Physics3D& p) { p.writebackTransform(dt); });
    characterComponents->forEach([](ecs::Entity, com::Physics3DCharacter& c) { c.postUpdate(); });
}

void Physics3D::process(const engine::event::WorldCreated& worldCreate) {
    engine::World3D* world = dynamic_cast<engine::World3D*>(&worldCreate.world);
    if (world) {
        if (world->worldIndex() >= activePhysicsSystems.size()) {
            activePhysicsSystems.resize(world->worldIndex() + 1, nullptr);
        }
        activePhysicsSystems[world->worldIndex()] = world;
        firstTickWorlds.push_back(world);
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
