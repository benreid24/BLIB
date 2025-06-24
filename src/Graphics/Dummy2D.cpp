#include <BLIB/Graphics/Dummy2D.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>

namespace bl
{
namespace gfx
{
void Dummy2D::create(engine::World& world) {
    createEntityOnly(world, ecs::Flags::Dummy);
    OverlayScalable::create(world.engine(), entity());
    rc::ovy::OverlayObject* obj =
        world.engine().ecs().emplaceComponent<rc::ovy::OverlayObject>(entity());
    obj->entity = entity();
}

void Dummy2D::setSize(const glm::vec2& size) { OverlayScalable::setLocalSize(size); }

} // namespace gfx
} // namespace bl
