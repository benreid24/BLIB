#include <BLIB/Engine/Player.hpp>

namespace bl
{
namespace engine
{
void Player::enterWorld(util::Ref<World> world) {
    World* oldWorld = worldStack.empty() ? nullptr : worldStack.top().get();
    worldStack.emplace(std::move(world));
    observer->pushScene(worldStack.top()->scene());
    onWorldChange(oldWorld, worldStack.top().get());
}

void Player::changeWorlds(util::Ref<World> world) {
    World* oldWorld = worldStack.empty() ? nullptr : worldStack.top().get();
    if (!worldStack.empty()) {
        worldStack.pop();
        observer->popScene();
    }
    worldStack.emplace(std::move(world));
    observer->pushScene(worldStack.top()->scene());
    onWorldChange(oldWorld, worldStack.top().get());
}

void Player::leaveWorld() {
    if (!worldStack.empty()) {
        World* oldWorld = worldStack.top().get();
        worldStack.pop();
        observer->popScene();
        onWorldChange(oldWorld, worldStack.empty() ? nullptr : worldStack.top().get());
    }
}

void Player::leaveAllWorlds() {
    World* oldWorld = worldStack.empty() ? nullptr : worldStack.top().get();
    while (!worldStack.empty()) { leaveWorld(); }
    onWorldChange(oldWorld, nullptr);
}

void Player::onWorldChange(World*, World*) {}

} // namespace engine
} // namespace bl
