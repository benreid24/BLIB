#include <BLIB/Graphics/Triangle.hpp>

namespace bl
{
namespace gfx
{
Triangle::Triangle()
: Shape2D()
, points{}
, colors{}
, hasColorOverride(false) {}

void Triangle::create(engine::Engine& engine, const glm::vec2& p0, const glm::vec2& p1,
                      const glm::vec2& p2) {
    points[0] = p0;
    points[1] = p1;
    points[2] = p2;
    Shape2D::create(engine);
}

void Triangle::create(engine::Engine& engine, const std::array<glm::vec2, 3>& p) {
    points[0] = p[0];
    points[1] = p[1];
    points[2] = p[2];
    Shape2D::create(engine);
}

void Triangle::setPoints(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2) {
    points[0] = p0;
    points[1] = p1;
    points[2] = p2;
    markDirty();
}

void Triangle::setPoints(const std::array<glm::vec2, 3>& p) {
    points[0] = p[0];
    points[1] = p[1];
    points[2] = p[2];
    markDirty();
}

void Triangle::setColorGradient(const glm::vec4& c0, const glm::vec4& c1, const glm::vec4& c2) {
    colors[0]        = c0;
    colors[1]        = c1;
    colors[2]        = c2;
    hasColorOverride = true;
    markDirty();
}

void Triangle::removeColorGradient() {
    hasColorOverride = false;
    markDirty();
}

unsigned int Triangle::getVertexCount() const { return 3; }

void Triangle::populateVertex(unsigned int i, rc::prim::Vertex& vertex) {
    vertex.pos = {points[i], 0.f};
    if (hasColorOverride) { vertex.color = colors[i]; }
}

} // namespace gfx
} // namespace bl
