#include <BLIB/Graphics/BatchCircle.hpp>

namespace bl
{
namespace gfx
{
BatchCircle::BatchCircle(unsigned int pointCount)
: CircleBase(pointCount) {}

void BatchCircle::create(engine::Engine& engine, BatchedShapes2D& owner, float r) {
    setRadius(r);
    BatchShape2D::create(engine, owner);
}

} // namespace gfx
} // namespace bl
