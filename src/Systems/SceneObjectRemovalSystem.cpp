#include <BLIB/Systems/SceneObjectRemovalSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
void SceneObjectRemovalSystem::init(engine::Engine& engine) {
    pool = &engine.renderer().scenePool();
}

void SceneObjectRemovalSystem::update(std::mutex&, float, float, float, float) {
    pool->performObjectRemovals();
}

} // namespace sys
} // namespace bl
