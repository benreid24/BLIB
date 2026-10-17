#include <BLIB/Objects/2D/DiscreteAnimationPlayer.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
namespace
{
bool modeToSlideshow(as::TypedRef<asi::Animation2DSetPayload> animation,
                     DiscreteAnimationPlayer::Mode mode) {
    switch (mode) {
    case DiscreteAnimationPlayer::Slideshow:
        return true;
    case DiscreteAnimationPlayer::VertexAnimation:
        return false;
    case DiscreteAnimationPlayer::Auto:
    default:
        return animation->isSlideshow();
    }
}
} // namespace

DiscreteAnimationPlayer::DiscreteAnimationPlayer()
: player(nullptr) {}

void DiscreteAnimationPlayer::create(engine::World& world,
                                     as::TypedRef<asi::Animation2DSetPayload> animation, Mode mode,
                                     bool play, bool forceLoop) {
    EntityBacked::createEntityOnly(world);
    player = world.engine().ecs().emplaceComponent<com::Animation2DPlayer>(
        entity(), animation, modeToSlideshow(animation, mode), play, forceLoop);
}

} // namespace o2d
} // namespace obj
} // namespace bl
