#include <BLIB/Graphics/Circle.hpp>

namespace bl
{
namespace gfx
{
Circle::Circle(unsigned int pointCount)
: CircleBase(pointCount) {}

void Circle::create(engine::World& world, float r) {
    setRadius(r);
    SingleShape2D::create(world);
}

void Circle::create(engine::World& world, ecs::Entity existing, float r) {
    setRadius(r);
    SingleShape2D::create(world, existing);
}

} // namespace gfx
} // namespace bl
