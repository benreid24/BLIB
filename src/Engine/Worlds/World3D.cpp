#include <BLIB/Engine/Worlds/World3D.hpp>

namespace bl
{
namespace engine
{
World3D::World3D(Engine& owner)
: BasicWorld(owner)
, navmesh(nullptr) {
    subscribe(owner.ecs().getSignalChannel());
}

World3D::~World3D() { unsubscribe(); }

void World3D::process(const ecs::event::ComponentAdded<com::Navmesh>& signal) {
    if (signal.entity.getWorldIndex() == worldIndex()) {
        if (!navmesh) { navmesh = &signal.component; }
        else { BL_LOG_WARN << "World3D already has a navmesh component, ignoring new one"; }
    }
}

void World3D::process(const ecs::event::ComponentRemoved<com::Navmesh>& signal) {
    if (navmesh == &signal.component) { navmesh = nullptr; }
}

} // namespace engine
} // namespace bl
