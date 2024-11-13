#include <BLIB/Graphics/Components/Animation2DPlayer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/World.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
Animation2DPlayer::Animation2DPlayer(bool fs)
: forSlideshow(fs)
, world(nullptr)
, me(ecs::InvalidEntity)
, playerEntity(ecs::InvalidEntity)
, player(nullptr) {}

void Animation2DPlayer::createNewPlayer(const resource::Ref<gfx::a2d::AnimationData>& animation,
                                        bool play, bool forceLoop) {
    // cleanup prior player. will be marked for removal only if other users still
    const bool isUpdate = cleanupPlayerDep();

    // create new player entity and component
    playerEntity = world->createEntity();
    player       = world->engine().ecs().emplaceComponent<com::Animation2DPlayer>(
        playerEntity, animation, forSlideshow, play, forceLoop);
    addPlayerDep();
    if (isUpdate) {
        Textured::setTexture(world->engine().renderer().texturePool().getOrLoadTexture(
            player->getAnimation()->resolvedSpritesheet()));
    }
}

void Animation2DPlayer::useExistingPlayer(ecs::Entity pent) {
    const bool isUpdate = cleanupPlayerDep();
    playerEntity        = pent;
    player              = world->engine().ecs().getComponent<com::Animation2DPlayer>(pent);
    addPlayerDep();
    if (isUpdate) {
        Textured::setTexture(world->engine().renderer().texturePool().getOrLoadTexture(
            player->getAnimation()->resolvedSpritesheet()));
    }
}

void Animation2DPlayer::create(engine::World& w, ecs::Entity entity, ecs::Entity pent) {
    world = &w;
    me    = entity;
    useExistingPlayer(pent);
    Textured::create(w.engine().ecs(),
                     entity,
                     world->engine().renderer().texturePool().getOrLoadTexture(
                         player->getAnimation()->resolvedSpritesheet()));
}

void Animation2DPlayer::create(engine::World& w, ecs::Entity entity,
                               const resource::Ref<gfx::a2d::AnimationData>& anim, bool play,
                               bool forceLoop) {
    world = &w;
    me    = entity;
    createNewPlayer(anim, play, forceLoop);
    Textured::create(w.engine().ecs(),
                     entity,
                     w.engine().renderer().texturePool().getOrLoadTexture(
                         player->getAnimation()->resolvedSpritesheet()));
}

void Animation2DPlayer::addPlayerDep() { world->engine().ecs().addDependency(playerEntity, me); }

bool Animation2DPlayer::cleanupPlayerDep() {
    if (world != nullptr && playerEntity != ecs::InvalidEntity && me != ecs::InvalidEntity) {
        world->engine().ecs().removeDependencyAndDestroyIfPossible(playerEntity, me);
        return true;
    }
    return false;
}

} // namespace bcom
} // namespace gfx
} // namespace bl
