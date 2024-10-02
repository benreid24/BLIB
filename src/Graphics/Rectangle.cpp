#include <BLIB/Graphics/Rectangle.hpp>

namespace bl
{
namespace gfx
{
Rectangle::Rectangle()
: RectangleBase() {}

void Rectangle::create(engine::World& world, const glm::vec2& s) {
    setSize(s);
    SingleShape2D::create(world);
}

void Rectangle::create(engine::World& world, ecs::Entity existingEntity, const glm::vec2& s) {
    setSize(s);
    SingleShape2D::create(world, existingEntity);
}

} // namespace gfx
} // namespace bl
