#include <BLIB/Systems/PathfindingSystem3D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
PathfindingSystem3D::PathfindingSystem3D()
: System(engine::FrameStage::Update0, engine::StateMask::Running)
, physicsPathers(nullptr) {}

void PathfindingSystem3D::init(engine::Engine& engine) {
    physicsPathers = &engine.ecs().getAllComponents<com::NavmeshPhysicsPather>();
}

void PathfindingSystem3D::update(std::mutex& stageMutex, float dt, float, float, float) {
    // TODO - pathing
}

void PathfindingSystem3D::process(const ecs::event::ComponentAdded<com::Navmesh>& signal) {
    const unsigned int world = signal.entity.getWorldIndex();
    if (world >= navmeshes.size()) { navmeshes.resize(world + 1); }

    if (navmeshes[world].navmesh != nullptr) {
        BL_LOG_WARN << "World " << world
                    << " already has a navmesh, multiple navmeshes per world are not yet supported";
        return;
    }

    navmeshes[world].navmesh = &signal.component;
}

void PathfindingSystem3D::process(const ecs::event::ComponentRemoved<com::Navmesh>& signal) {
    const unsigned int world = signal.entity.getWorldIndex();
    if (world >= navmeshes.size()) { return; }

    if (navmeshes[world].navmesh == &signal.component) { navmeshes[world].navmesh = nullptr; }
}

} // namespace sys
} // namespace bl