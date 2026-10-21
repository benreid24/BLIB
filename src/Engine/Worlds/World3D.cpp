#include <BLIB/Engine/Worlds/World3D.hpp>

#include <BLIB/Systems/PathfindingSystem3D.hpp>
#include <BLIB/Systems/Physics3D.hpp>
#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

namespace bl
{
namespace engine
{
World3D::World3D(Engine& owner)
: BasicWorld(owner) {
    owner.systems().registerSystemIfNotExists<sys::Physics3D>();
    owner.systems().registerSystemIfNotExists<sys::PathfindingSystem3D>();
    owner.systems().registerSystemIfNotExists<sys::SkeletalAnimationSystem>();
}

World3D::~World3D() {}

} // namespace engine
} // namespace bl
