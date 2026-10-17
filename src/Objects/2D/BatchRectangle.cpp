#include <BLIB/Objects/2D/BatchRectangle.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
BatchRectangle::BatchRectangle()
: RectangleBase() {}

void BatchRectangle::create(engine::Engine& engine, BatchedShapes& owner, const glm::vec2& s) {
    setSize(s);
    BatchShape::create(engine, owner);
}

} // namespace o2d
} // namespace obj
} // namespace bl
