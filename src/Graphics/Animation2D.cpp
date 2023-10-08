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

Animation2D::Animation2D(engine::Engine& engine, const resource::Ref<a2d::AnimationData>& animation,
                         bool play, bool forceLoop)
: Animation2D() {
    createWithUniquePlayer(engine, animation, play, forceLoop);
}

Animation2D::Animation2D(engine::Engine& engine, const Animation2D& player)
: Animation2D() {
    createWithSharedPlayer(engine, player);
}

void Animation2D::createWithUniquePlayer(engine::Engine& engine,
                                         const resource::Ref<a2d::AnimationData>& animation,
                                         bool play, bool forceLoop) {
    Drawable::create(engine);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(getSize(animation));
    Animation2DPlayer::create(
        engine.renderer(), engine.ecs(), entity(), animation, play, forceLoop);
    component().create(engine, Animation2DPlayer::getPlayer());
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

void Animation2D::createWithSharedPlayer(engine::Engine& engine, const Animation2D& player) {
    Drawable::create(engine);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(player.getLocalSize());
    Animation2DPlayer::create(engine.renderer(), engine.ecs(), entity(), player.getPlayerEntity());
    component().create(engine, Animation2DPlayer::getPlayer());
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

} // namespace gfx
} // namespace bl
