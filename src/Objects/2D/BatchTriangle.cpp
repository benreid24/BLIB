#include <BLIB/Objects/2D/BatchTriangle.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
BatchTriangle::BatchTriangle()
: TriangleBase() {}

void BatchTriangle::create(engine::Engine& engine, BatchedShapes& owner, const glm::vec2& p0,
                           const glm::vec2& p1, const glm::vec2& p2) {
    setPoints(p0, p1, p2);
    BatchShape::create(engine, owner);
}

void BatchTriangle::create(engine::Engine& engine, BatchedShapes& owner,
                           const std::array<glm::vec2, 3>& p) {
    setPoints(p);
    BatchShape::create(engine, owner);
}

} // namespace o2d
} // namespace obj
} // namespace bl
