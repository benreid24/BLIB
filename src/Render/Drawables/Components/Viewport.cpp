#include <BLIB/Render/Drawables/Components/Viewport.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
void Viewport::setViewport(const ovy::Viewport& vp) {
    registry->emplaceComponent<ovy::Viewport>(ecsId, vp);
}

void Viewport::clearViewport() { registry->removeComponent<ovy::Viewport>(ecsId); }

void Viewport::create(ecs::Registry& r, ecs::Entity ent) {
    registry = &r;
    ecsId    = ent;
}

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl
