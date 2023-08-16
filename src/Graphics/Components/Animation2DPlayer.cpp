#include <BLIB/Graphics/Components/Animation2DPlayer.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
void Animation2DPlayer::createNewPlayer(const resource::Ref<gfx::a2d::AnimationData>& animation,
                                        bool play, bool forceLoop) {
    player = registry->emplaceComponent<com::Animation2DPlayer>(me, animation, play, forceLoop);
}

void Animation2DPlayer::useExistingPlayer(ecs::Entity pent) {
    player = registry->getComponent<com::Animation2DPlayer>(pent);
}

void Animation2DPlayer::create(rc::Renderer& r, ecs::Registry& reg, ecs::Entity entity,
                               ecs::Entity pent) {
    renderer = &r;
    registry = &reg;
    me       = entity;
    player   = registry->getComponent<com::Animation2DPlayer>(pent);
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
        renderer->texturePool().getOrLoadTexture(player.get().animation->resolvedSpritesheet()));
}

} // namespace bcom
} // namespace gfx
} // namespace bl
