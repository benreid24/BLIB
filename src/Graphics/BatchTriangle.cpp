#include <BLIB/Graphics/BatchTriangle.hpp>

namespace bl
{
namespace gfx
{
BatchTriangle::BatchTriangle()
: TriangleBase() {}

void BatchTriangle::create(engine::Engine& engine, BatchedShapes2D& owner, const glm::vec2& p0,
                           const glm::vec2& p1, const glm::vec2& p2) {
    setPoints(p0, p1, p2);
    BatchShape2D::create(engine, owner);
}

void BatchTriangle::create(engine::Engine& engine, BatchedShapes2D& owner,
                           const std::array<glm::vec2, 3>& p) {
    setPoints(p);
    BatchShape2D::create(engine, owner);
}

} // namespace gfx
} // namespace bl
