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
                       sys::jolt::Constants::MaxBodyPairs,
                       sys::jolt::Constants::MaxContactConstraints,
                       sys::jolt::Constants::MaxBodyMutexes,
                       physics.getBroadphaseLayerInterface(),
                       physics.getObjectLayerVsBroadphaseFilter(),
                       physics.getObjectLayerPairFilter());
}

World3D::~World3D() {}

} // namespace engine
} // namespace bl
