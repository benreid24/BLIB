#include <BLIB/Graphics/Rectangle.hpp>

namespace bl
{
namespace gfx
{
Rectangle::Rectangle()
: RectangleBase() {}

void Rectangle::create(engine::Engine& engine, const glm::vec2& s) {
    setSize(s);
    SingleShape2D::create(engine);
}

} // namespace gfx
} // namespace bl
