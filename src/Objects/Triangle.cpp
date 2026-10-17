#include <BLIB/Objects/Triangle.hpp>

namespace bl
{
namespace obj
{
Triangle::Triangle()
: TriangleBase() {}

void Triangle::create(engine::World& world, const glm::vec2& p0, const glm::vec2& p1,
                      const glm::vec2& p2) {
    setPoints(p0, p1, p2);
    SingleShape2D::create(world);
}

void Triangle::create(engine::World& world, const std::array<glm::vec2, 3>& p) {
    setPoints(p);
    SingleShape2D::create(world);
}

} // namespace obj
} // namespace bl
