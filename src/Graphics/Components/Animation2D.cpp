#include <BLIB/Graphics/Components/Animation2D.hpp>

namespace bl
{
namespace gfx
{
namespace base
{
void Animation2D::createNewPlayer(const resource::Ref<gfx::a2d::AnimationData>& animation,
                                  bool play, bool forceLoop) {
    player = registry->emplaceComponent<com::Animation2DPlayer>(me, animation, play, forceLoop);
}

void Animation2D::useExistingPlayer(ecs::Entity pent) {
    animation->setPlayer(pent);
    player = registry->getComponent<com::Animation2DPlayer>(pent);
}

void Animation2D::create(ecs::Registry& reg, ecs::Entity entity, ecs::Entity pent) {
    registry  = &reg;
    me        = entity;
    animation = reg.emplaceComponent<com::Animation2D>(entity, pent);
    player    = reg.getComponent<com::Animation2DPlayer>(pent);
}

void Animation2D::create(ecs::Registry& reg, ecs::Entity entity,
                         const resource::Ref<gfx::a2d::AnimationData>& anim, bool play,
                         bool forceLoop) {
    registry = &reg;
    me       = entity;
    createNewPlayer(anim, play, forceLoop);
    create(reg, entity, entity);
}

} // namespace base
} // namespace gfx
} // namespace bl
