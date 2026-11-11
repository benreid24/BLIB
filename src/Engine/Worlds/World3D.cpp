#include <BLIB/Engine/Worlds/World3D.hpp>

#include <BLIB/Systems/Jolt/Constants.hpp>
#include <BLIB/Systems/PathfindingSystem3D.hpp>
#include <BLIB/Systems/Physics3D.hpp>
#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

namespace bl
{
namespace engine
{

World3D::World3D(Engine& owner)
: BasicWorld(owner) {
    auto& physics = owner.systems().registerSystemIfNotExists<sys::Physics3D>();
    owner.systems().registerSystemIfNotExists<sys::PathfindingSystem3D>();
    owner.systems().registerSystemIfNotExists<sys::SkeletalAnimationSystem>();

    physicsSystem.Init(sys::jolt::Constants::MaxBodies,
                       sys::jolt::Constants::MaxBodyMutexes,
                       sys::jolt::Constants::MaxBodyPairs,
                       sys::jolt::Constants::MaxContactConstraints,
                       physics.getBroadphaseLayerInterface(),
                       physics.getObjectLayerVsBroadphaseFilter(),
                       physics.getObjectLayerPairFilter());
    contactListener.init(physics.getSignalChannel(), physicsSystem.GetBodyInterfaceNoLock());
    physicsSystem.SetContactListener(&contactListener);
}

World3D::~World3D() {
    // need to delete here because Physics3D component references us
    destroyAllEntities();
    engine().ecs().flushDeletions();
}

void World3D::queueAddPhysicsBody(JPH::BodyID bodyId, JPH::EActivation activation) {
    if (activation == JPH::EActivation::Activate) { bodiesToAddAndActivate.push_back(bodyId); }
    else { bodiesToAddNoActivate.push_back(bodyId); }
}

void World3D::queueRemovePhysicsBody(JPH::BodyID bodyId) { bodiesToRemove.push_back(bodyId); }

void World3D::queueDestroyPhysicsBody(JPH::BodyID bodyId) { bodiesToDestroy.push_back(bodyId); }

void World3D::drainPhysicsQueues() {
    auto& body = physicsSystem.GetBodyInterface();

    if (!bodiesToRemove.empty()) {
        body.RemoveBodies(bodiesToRemove.data(), bodiesToRemove.size());
        bodiesToRemove.clear();
    }

    if (!bodiesToDestroy.empty()) {
        body.DestroyBodies(bodiesToDestroy.data(), bodiesToDestroy.size());
        bodiesToDestroy.clear();
    }

    if (!bodiesToAddAndActivate.empty()) {
        auto state =
            body.AddBodiesPrepare(bodiesToAddAndActivate.data(), bodiesToAddAndActivate.size());
        body.AddBodiesFinalize(bodiesToAddAndActivate.data(),
                               bodiesToAddAndActivate.size(),
                               state,
                               JPH::EActivation::Activate);
        bodiesToAddAndActivate.clear();
    }

    if (!bodiesToAddNoActivate.empty()) {
        auto state =
            body.AddBodiesPrepare(bodiesToAddNoActivate.data(), bodiesToAddNoActivate.size());
        body.AddBodiesFinalize(bodiesToAddNoActivate.data(),
                               bodiesToAddNoActivate.size(),
                               state,
                               JPH::EActivation::DontActivate);
        bodiesToAddNoActivate.clear();
    }
}

} // namespace engine
} // namespace bl
