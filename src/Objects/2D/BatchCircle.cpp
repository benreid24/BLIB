#include <BLIB/Objects/2D/BatchCircle.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
BatchCircle::BatchCircle(unsigned int pointCount)
: CircleBase(pointCount) {}

void BatchCircle::create(engine::Engine& engine, BatchedShapes& owner, float r) {
    setRadius(r);
    BatchShape::create(engine, owner);
}

} // namespace o2d
} // namespace obj
} // namespace bl
