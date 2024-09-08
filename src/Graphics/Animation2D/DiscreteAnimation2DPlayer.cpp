#include <BLIB/Graphics/Animation2D/DiscreteAnimation2DPlayer.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
namespace
{
bool modeToSlideshow(const resource::Ref<gfx::a2d::AnimationData>& animation,
                     DiscreteAnimation2DPlayer::Mode mode) {
    switch (mode) {
    case DiscreteAnimation2DPlayer::Slideshow:
        return true;
    case DiscreteAnimation2DPlayer::VertexAnimation:
        return false;
    case DiscreteAnimation2DPlayer::Auto:
    default:
        return animation->isSlideshow();
    }
}
} // namespace

DiscreteAnimation2DPlayer::DiscreteAnimation2DPlayer()
: player(nullptr) {}

void DiscreteAnimation2DPlayer::create(engine::World& world,
                                       const resource::Ref<a2d::AnimationData>& animation,
                                       Mode mode, bool play, bool forceLoop) {
    EntityBacked::createEntityOnly(world);
    player = world.engine().ecs().emplaceComponent<com::Animation2DPlayer>(
        entity(), animation, modeToSlideshow(animation, mode), play, forceLoop);
}

} // namespace gfx
} // namespace bl
