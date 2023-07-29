#include <BLIB/Systems/Animation2DSystem.hpp>

#include <BLIB/Engine.hpp>

namespace bl
{
namespace sys
{
Animation2DSystem::Animation2DSystem()
: players(nullptr) {}

void Animation2DSystem::init(engine::Engine& engine) {
    players = &engine.ecs().getAllComponents<com::Animation2DPlayer>();
}

void Animation2DSystem::update(std::mutex&, float dt) {
    players->forEach([dt](ecs::Entity, com::Animation2DPlayer& player) { player.update(dt); });
}

} // namespace sys
} // namespace bl
