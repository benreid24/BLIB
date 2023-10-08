#include <BLIB/Graphics/Components/Animation2DPlayer.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
Animation2DPlayer::Animation2DPlayer()
: renderer(nullptr)
, registry(nullptr)
, me(ecs::InvalidEntity)
, playerEntity(ecs::InvalidEntity)
, player(nullptr) {}

void Animation2DPlayer::createNewPlayer(const resource::Ref<gfx::a2d::AnimationData>& animation,
                                        bool play, bool forceLoop) {
    // cleanup prior player. will be marked for removal only if other users still
    cleanupPlayerDep();

    // create new player entity and component
    playerEntity = registry->createEntity();
    player       = registry->emplaceComponent<com::Animation2DPlayer>(
        playerEntity, animation, play, forceLoop);
    addPlayerDep();
}

void Animation2DPlayer::useExistingPlayer(ecs::Entity pent) {
    playerEntity = pent;
    player       = registry->getComponent<com::Animation2DPlayer>(pent);
    addPlayerDep();
}

void Animation2DPlayer::create(rc::Renderer& r, ecs::Registry& reg, ecs::Entity entity,
                               ecs::Entity pent) {
    renderer = &r;
    registry = &reg;
    me       = entity;
    useExistingPlayer(pent);
    Textured::create(
        reg,
        entity,
        renderer->texturePool().getOrLoadTexture(player->animation->resolvedSpritesheet()));
}

void Animation2DPlayer::create(rc::Renderer& r, ecs::Registry& reg, ecs::Entity entity,
                               const resource::Ref<gfx::a2d::AnimationData>& anim, bool play,
                               bool forceLoop) {
    renderer = &r;
    registry = &reg;
    me       = entity;
    createNewPlayer(anim, play, forceLoop);
    Textured::create(
        reg,
        entity,
        renderer->texturePool().getOrLoadTexture(player->animation->resolvedSpritesheet()));
}

void Animation2DPlayer::addPlayerDep() { registry->addDependency(playerEntity, me); }

void Animation2DPlayer::cleanupPlayerDep() {
    if (registry != nullptr && playerEntity != ecs::InvalidEntity && me != ecs::InvalidEntity) {
        registry->removeDependencyAndDestroyIfPossible(playerEntity, me);
    }
}

} // namespace bcom
} // namespace gfx
} // namespace bl
