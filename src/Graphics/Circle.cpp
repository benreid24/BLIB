#include <BLIB/Graphics/Circle.hpp>

#include <BLIB/Math.hpp>
#include <cmath>

namespace bl
{
namespace gfx
{
gfx::Circle::Circle(unsigned int pointCount)
: Shape2D()
, pointCount(pointCount)
, radius(100.f)
, hasColorOverride(false)
, centerColor{}
, edgeColor{} {}

void gfx::Circle::create(engine::Engine& engine, float r) {
    radius = r;
    Shape2D::create(engine);
    OverlayScalable::setLocalSize({r * 2.f, 2 * 2.f});
}

void gfx::Circle::setRadius(float r) {
    radius = r;
    OverlayScalable::setLocalSize({r * 2.f, 2 * 2.f});
    markDirty();
}

float gfx::Circle::getRadius() const { return radius; }

void gfx::Circle::setPointCount(unsigned int pc) {
    pointCount = pc;
    markDirty();
}

void gfx::Circle::setColorGradient(const glm::vec4& c, const glm::vec4& e) {
    hasColorOverride = true;
    centerColor      = c;
    edgeColor        = e;
    markDirty();
}

void gfx::Circle::removeColorGradient() {
    hasColorOverride = false;
    markDirty();
}

unsigned int gfx::Circle::getVertexCount() const {
    return pointCount + 1; // repeat first point
}

void gfx::Circle::populateVertex(unsigned int index, rc::prim::Vertex& vertex) {
    if (hasColorOverride) { vertex.color = edgeColor; }

    const float percent = static_cast<float>(index) / static_cast<float>(pointCount);
    const float radians = percent * 2.f * math::Pi;
    vertex.pos.x        = std::cos(radians) * radius + radius;
    vertex.pos.y        = std::sin(radians) * radius + radius;
    vertex.pos.z        = 0.f;
}

glm::vec4 gfx::Circle::getCenterColor(const glm::vec4& avgColor) const {
    return hasColorOverride ? centerColor : avgColor;
}

} // namespace gfx
} // namespace bl
