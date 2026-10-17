#include <BLIB/Objects/2D/Circle.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
Circle::Circle(unsigned int pointCount)
: CircleBase(pointCount) {}

void Circle::create(engine::World& world, float r) {
    setRadius(r);
    SingleShape::create(world);
}

void Circle::create(engine::World& world, ecs::Entity existing, float r) {
    setRadius(r);
    SingleShape::create(world, existing);
}

} // namespace o2d
} // namespace obj
} // namespace bl
