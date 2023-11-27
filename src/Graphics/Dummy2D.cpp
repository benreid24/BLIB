#include <BLIB/Graphics/Dummy2D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
Dummy2D::Dummy2D()
: hidden(false)
, object(nullptr) {}

void Dummy2D::create(engine::Engine& engine) {
    createEntityOnly(engine, ecs::Flags::Dummy);
    OverlayScalable::create(engine, entity());
    object         = engine.ecs().emplaceComponent<rc::ovy::OverlayObject>(entity());
    object->entity = entity();
    object->hidden = hidden;
}

void Dummy2D::setSize(const glm::vec2& size) { OverlayScalable::setLocalSize(size); }

void Dummy2D::setHidden(bool h) {
    if (object) { object->hidden = h; }
}

} // namespace gfx
} // namespace bl
