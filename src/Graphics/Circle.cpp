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

void Circle::create(engine::Engine& engine, ecs::Entity existing, float r) {
    setRadius(r);
    SingleShape2D::create(engine, existing);
}

} // namespace gfx
} // namespace bl
