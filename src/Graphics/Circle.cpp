#include <BLIB/Graphics/Circle.hpp>

namespace bl
{
namespace gfx
{
Circle::Circle(unsigned int pointCount)
: CircleBase(pointCount) {}

void Circle::create(engine::Engine& engine, float r) {
    setRadius(r);
    SingleShape2D::create(engine);
}

} // namespace gfx
} // namespace bl
