#include <BLIB/Graphics/Slideshow.hpp>

namespace bl
{
namespace gfx
{
namespace
{
glm::vec2 getSize(const resource::Ref<a2d::AnimationData>& animation) {
    if (!animation || animation->frameCount() == 0 || !animation->isSlideshow()) {
        return {1.f, 1.f};
    }
    const sf::Vector2f s = animation->getFrameSize(0);
    return {s.x, s.y};
}
} // namespace

Slideshow::Slideshow()
: Animation2DPlayer(true) {}

Slideshow::Slideshow(engine::World& world, const resource::Ref<a2d::AnimationData>& animation,
                     bool play, bool forceLoop)
: Slideshow() {
    createWithUniquePlayer(world, animation, play, forceLoop);
}

Slideshow::Slideshow(engine::World& world, ecs::Entity existing,
                     const resource::Ref<a2d::AnimationData>& animation, bool play, bool forceLoop)
: Slideshow() {
    createWithUniquePlayer(world, existing, animation, play, forceLoop);
}

Slideshow::Slideshow(engine::World& world, const Slideshow& player)
: Slideshow() {
    createWithSharedPlayer(world, player);
}

Slideshow::Slideshow(engine::World& world, ecs::Entity existing, const Slideshow& player)
: Slideshow() {
    createWithSharedPlayer(world, existing, player);
}

void Slideshow::createWithUniquePlayer(engine::World& world,
                                       const resource::Ref<a2d::AnimationData>& animation,
                                       bool play, bool forceLoop) {
    Drawable::create(world);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(world, entity(), material(), animation, play, forceLoop);
    component().create(world.engine().renderer().vulkanState(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Slideshow::createWithUniquePlayer(engine::World& world, ecs::Entity existing,
                                       const resource::Ref<a2d::AnimationData>& animation,
                                       bool play, bool forceLoop) {
    Drawable::createComponentOnly(world, existing);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(world, entity(), material(), animation, play, forceLoop);
    component().create(world.engine().renderer().vulkanState(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Slideshow::createWithSharedPlayer(engine::World& world, const Slideshow& player) {
    Drawable::create(world);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(world, entity(), player.getPlayerEntity(), material());
    component().create(world.engine().renderer().vulkanState(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Slideshow::createWithSharedPlayer(engine::World& world, ecs::Entity existing,
                                       const Slideshow& player) {
    Drawable::createComponentOnly(world, existing);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(world, entity(), player.getPlayerEntity(), material());
    component().create(world.engine().renderer().vulkanState(), Animation2DPlayer::getPlayer());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Slideshow::scaleToSize(const glm::vec2& size) {
    getTransform().setScale(size / OverlayScalable::getLocalSize());
}

void Slideshow::ensureLocalSizeUpdated() {}

} // namespace gfx
} // namespace bl
