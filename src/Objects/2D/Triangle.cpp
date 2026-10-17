#include <BLIB/Objects/2D/Triangle.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
Triangle::Triangle()
: TriangleBase() {}

void Triangle::create(engine::World& world, const glm::vec2& p0, const glm::vec2& p1,
                      const glm::vec2& p2) {
    setPoints(p0, p1, p2);
    SingleShape::create(world);
}

void Triangle::create(engine::World& world, const std::array<glm::vec2, 3>& p) {
    setPoints(p);
    SingleShape::create(world);
}

} // namespace o2d
} // namespace obj
} // namespace bl
