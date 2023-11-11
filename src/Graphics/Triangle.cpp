#include <BLIB/Graphics/Triangle.hpp>

namespace bl
{
namespace gfx
{
Triangle::Triangle()
: TriangleBase() {}

void Triangle::create(engine::Engine& engine, const glm::vec2& p0, const glm::vec2& p1,
                      const glm::vec2& p2) {
    setPoints(p0, p1, p2);
    SingleShape2D::create(engine);
}

void Triangle::create(engine::Engine& engine, const std::array<glm::vec2, 3>& p) {
    setPoints(p);
    SingleShape2D::create(engine);
}

} // namespace gfx
} // namespace bl
