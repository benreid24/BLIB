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

Slideshow::Slideshow(engine::Engine& engine, const resource::Ref<a2d::AnimationData>& animation,
                     bool play, bool forceLoop)
: Slideshow() {
    createWithUniquePlayer(engine, animation, play, forceLoop);
}

Slideshow::Slideshow(engine::Engine& engine, const Slideshow& player)
: Slideshow() {
    createWithSharedPlayer(engine, player);
}

void Slideshow::createWithUniquePlayer(engine::Engine& engine,
                                       const resource::Ref<a2d::AnimationData>& animation,
                                       bool play, bool forceLoop) {
    Drawable::create(engine);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(
        engine.renderer(), engine.ecs(), entity(), animation, play, forceLoop);
    component().create(engine.renderer().vulkanState(), Animation2DPlayer::getPlayer());
}

void Slideshow::createWithSharedPlayer(engine::Engine& engine, const Slideshow& player) {
    Drawable::create(engine);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(engine.renderer(), engine.ecs(), entity(), player.entity());
    component().create(engine.renderer().vulkanState(), Animation2DPlayer::getPlayer());
}

} // namespace gfx
} // namespace bl
