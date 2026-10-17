#include <BLIB/Objects/BatchCircle.hpp>

namespace bl
{
namespace obj
{
BatchCircle::BatchCircle(unsigned int pointCount)
: CircleBase(pointCount) {}

void BatchCircle::create(engine::Engine& engine, BatchedShapes2D& owner, float r) {
    setRadius(r);
    BatchShape2D::create(engine, owner);
}

} // namespace obj
} // namespace bl
