#include <BLIB/Objects/2D/Dummy.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
void Dummy::create(engine::World& world) {
    createEntityOnly(world, ecs::Flags::Dummy);
    OverlayScalable::create(world.engine(), entity());
    rc::ovy::OverlayObject* obj =
        world.engine().ecs().emplaceComponent<rc::ovy::OverlayObject>(entity());
    obj->entity = entity();
}

void Dummy::setSize(const glm::vec2& size) { OverlayScalable::setLocalSize(size); }

} // namespace o2d
} // namespace obj
} // namespace bl
