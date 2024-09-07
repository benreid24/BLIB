#include <BLIB/Engine/Player.hpp>

namespace bl
{
namespace engine
{
void Player::enterWorld(util::Ref<World> world) {
    worldStack.emplace(std::move(world));
    observer->pushScene(worldStack.top()->scene());
}

void Player::changeWorlds(util::Ref<World> world) {
    if (!worldStack.empty()) {
        worldStack.pop();
        observer->popScene();
    }
    worldStack.emplace(std::move(world));
    observer->pushScene(worldStack.top()->scene());
}

void Player::leaveWorld() {
    if (!worldStack.empty()) {
        worldStack.pop();
        observer->popScene();
    }
}

void Player::leaveAllWorlds() {
    while (!worldStack.empty()) { leaveWorld(); }
}

} // namespace engine
} // namespace bl
