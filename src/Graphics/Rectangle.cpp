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

} // namespace gfx
} // namespace bl
