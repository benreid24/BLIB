#include <BLIB/Graphics/Dummy2D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
void Dummy2D::create(engine::Engine& engine) {
    createEntityOnly(engine, ecs::Flags::Dummy);
    OverlayScalable::create(engine, entity());
}

void Dummy2D::setSize(const glm::vec2& size) { OverlayScalable::setLocalSize(size); }

} // namespace gfx
} // namespace bl
