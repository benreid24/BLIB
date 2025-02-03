#include <BLIB/Graphics/Animation2D.hpp>

namespace bl
{
namespace gfx
{
namespace
{
glm::vec2 getSize(const resource::Ref<a2d::AnimationData>& animation) {
    if (!animation || animation->frameCount() == 0) { return {1.f, 1.f}; }
    sf::Vector2f s = animation->getFrameSize(0);
    for (unsigned int i = 1; i < animation->frameCount(); ++i) {
        s.x = std::max(s.x, animation->getFrame(i).size.x);
        s.y = std::max(s.y, animation->getFrame(i).size.y);
    }
    return {s.x, s.y};
}
} // namespace

Animation2D::Animation2D()
: Animation2DPlayer(false) {}

Animation2D::Animation2D(engine::World& world, const resource::Ref<a2d::AnimationData>& animation,
                         bool play, bool forceLoop)
: Animation2D() {
    createWithUniquePlayer(world, animation, play, forceLoop);
}

Animation2D::Animation2D(engine::World& world, ecs::Entity existing,
                         const resource::Ref<a2d::AnimationData>& animation, bool play,
                         bool forceLoop)
: Animation2D() {
    createWithUniquePlayer(world, existing, animation, play, forceLoop);
}

Animation2D::Animation2D(engine::World& world, const Animation2D& player)
: Animation2D() {
    createWithSharedPlayer(world, player);
}

Animation2D::Animation2D(engine::World& world, ecs::Entity existing, const Animation2D& player)
: Animation2D() {
    createWithSharedPlayer(world, existing, player);
}

void Animation2D::createWithUniquePlayer(engine::World& world,
                                         const resource::Ref<a2d::AnimationData>& animation,
                                         bool play, bool forceLoop) {
    Drawable::create(world);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(world, entity(), material(), animation, play, forceLoop);
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation2D::createWithUniquePlayer(engine::World& world, ecs::Entity existing,
                                         const resource::Ref<a2d::AnimationData>& animation,
                                         bool play, bool forceLoop) {
    Drawable::createComponentOnly(world, existing);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(world, entity(), material(), animation, play, forceLoop);
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation2D::createWithSharedPlayer(engine::World& world, const Animation2D& player) {
    Drawable::create(world);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(world, entity(), player.getPlayerEntity(), material());
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation2D::createWithSharedPlayer(engine::World& world, ecs::Entity existing,
                                         const Animation2D& player) {
    Drawable::createComponentOnly(world, existing);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(world, entity(), player.getPlayerEntity(), material());
    component().create(world.engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation2D::setAnimationWithUniquePlayer(const resource::Ref<a2d::AnimationData>& animation,
                                               bool play, bool forceLoop) {
    Animation2DPlayer::createNewPlayer(animation, play, forceLoop);
    component().create(engine(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Animation2D::scaleToSize(const glm::vec2& size) {
    getTransform().setScale(size / getLocalSize());
}

void Animation2D::ensureLocalSizeUpdated() {
    if (entity() != ecs::InvalidEntity && component().player &&
        component().player->getAnimation()) {
        const auto sf = component().player->getAnimation()->getMaxSize();
        OverlayScalable::setLocalSize({sf.x, sf.y});
    }
}

} // namespace gfx
} // namespace bl
