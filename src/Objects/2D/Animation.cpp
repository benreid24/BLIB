#include <BLIB/Objects/2D/Animation.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
namespace
{
glm::vec2 getSize(as::TypedRef<asi::Animation2DSetPayload> animation) {
    if (!animation || animation->frameCount() == 0) { return {1.f, 1.f}; }
    glm::vec2 s = animation->getFrameSize(0);
    for (unsigned int i = 1; i < animation->frameCount(); ++i) {
        s.x = std::max(s.x, animation->getFrame(i).size.x);
        s.y = std::max(s.y, animation->getFrame(i).size.y);
    }
    return {s.x, s.y};
}
} // namespace

Animation::Animation()
: Animation2DPlayer(false) {}

Animation::Animation(engine::World& world, as::TypedRef<asi::Animation2DSetPayload> animation,
                     bool play, bool forceLoop)
: Animation() {
    createWithUniquePlayer(world, animation, play, forceLoop);
}

Animation::Animation(engine::World& world, ecs::Entity existing,
                     as::TypedRef<asi::Animation2DSetPayload> animation, bool play, bool forceLoop)
: Animation() {
    createWithUniquePlayer(world, existing, animation, play, forceLoop);
}

Animation::Animation(engine::World& world, const Animation& player)
: Animation() {
    createWithSharedPlayer(world, player);
}

Animation::Animation(engine::World& world, ecs::Entity existing, const Animation& player)
: Animation() {
    createWithSharedPlayer(world, existing, player);
}

void Animation::createWithUniquePlayer(engine::World& world,
                                       as::TypedRef<asi::Animation2DSetPayload> animation,
                                       bool play, bool forceLoop) {
    Drawable::create(world);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(world, entity(), material(), animation, play, forceLoop);
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation::createWithUniquePlayer(engine::World& world, ecs::Entity existing,
                                       as::TypedRef<asi::Animation2DSetPayload> animation,
                                       bool play, bool forceLoop) {
    Drawable::createComponentOnly(world, existing);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(world, entity(), material(), animation, play, forceLoop);
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation::createWithSharedPlayer(engine::World& world, const Animation& player) {
    Drawable::create(world);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(world, entity(), player.getPlayerEntity(), material());
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation::createWithSharedPlayer(engine::World& world, ecs::Entity existing,
                                       const Animation& player) {
    Drawable::createComponentOnly(world, existing);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(world, entity(), player.getPlayerEntity(), material());
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation::setAnimationWithUniquePlayer(as::TypedRef<asi::Animation2DSetPayload> animation,
                                             bool play, bool forceLoop) {
    Animation2DPlayer::createNewPlayer(animation, play, forceLoop);
    component().create(engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation::scaleToSize(const glm::vec2& size) {
    getTransform().setScale(size / getLocalSize());
}

void Animation::ensureLocalSizeUpdated() {
    if (entity() != ecs::InvalidEntity && component().player &&
        component().player->getAnimation()) {
        const auto sf = component().player->getAnimation()->getMaxSize();
        OverlayScalable::setLocalSize({sf.x, sf.y});
    }
}

} // namespace o2d
} // namespace obj
} // namespace bl
