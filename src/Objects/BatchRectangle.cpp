#include <BLIB/Objects/BatchRectangle.hpp>

namespace bl
{
namespace obj
{
BatchRectangle::BatchRectangle()
: RectangleBase() {}

void BatchRectangle::create(engine::Engine& engine, BatchedShapes2D& owner, const glm::vec2& s) {
    setSize(s);
    BatchShape2D::create(engine, owner);
}

} // namespace obj
} // namespace bl
