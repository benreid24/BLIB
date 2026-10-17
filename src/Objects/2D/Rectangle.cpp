#include <BLIB/Objects/2D/Rectangle.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
Rectangle::Rectangle()
: RectangleBase() {}

void Rectangle::create(engine::World& world, const glm::vec2& s) {
    setSize(s);
    SingleShape::create(world);
}

void Rectangle::create(engine::World& world, ecs::Entity existingEntity, const glm::vec2& s) {
    setSize(s);
    SingleShape::create(world, existingEntity);
}

} // namespace o2d
} // namespace obj
} // namespace bl
