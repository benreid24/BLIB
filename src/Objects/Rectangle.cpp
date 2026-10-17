#include <BLIB/Objects/Rectangle.hpp>

namespace bl
{
namespace obj
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

} // namespace obj
} // namespace bl
