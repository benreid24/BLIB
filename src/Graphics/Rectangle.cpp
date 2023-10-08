#include <BLIB/Graphics/Rectangle.hpp>

namespace bl
{
namespace gfx
{
Rectangle::Rectangle()
: Shape2D()
, size(50.f, 50.f)
, colorOverrides{}
, hasGradient(false) {}

void Rectangle::create(engine::Engine& engine, const glm::vec2& s) {
    size = s;
    Shape2D::create(engine);
}

void Rectangle::setSize(const glm::vec2& s) {
    OverlayScalable::setLocalSize(s);
    size = s;
    markDirty();
}

const glm::vec2& Rectangle::getSize() const { return size; }

void Rectangle::setColorGradient(const glm::vec4& topLeft, const glm::vec4& topRight,
                                 const glm::vec4& bottomRight, const glm::vec4& bottomLeft) {
    colorOverrides[0] = topLeft;
    colorOverrides[1] = topRight;
    colorOverrides[2] = bottomRight;
    colorOverrides[3] = bottomLeft;
    hasGradient       = true;
    markDirty();
}

void Rectangle::setHorizontalColorGradient(const glm::vec4& leftColor,
                                           const glm::vec4& rightColor) {
    colorOverrides[0] = leftColor;
    colorOverrides[1] = rightColor;
    colorOverrides[2] = rightColor;
    colorOverrides[3] = leftColor;
    hasGradient       = true;
    markDirty();
}

void Rectangle::setVerticalColorGradient(const glm::vec4& topColor, const glm::vec4& bottomColor) {
    colorOverrides[0] = topColor;
    colorOverrides[1] = topColor;
    colorOverrides[2] = bottomColor;
    colorOverrides[3] = bottomColor;
    hasGradient       = true;
    markDirty();
}

void Rectangle::removeColorGradient() {
    hasGradient = false;
    markDirty();
}

unsigned int Rectangle::getVertexCount() const { return 4; }

void Rectangle::populateVertex(unsigned int i, rc::prim::Vertex& vertex) {
    switch (i) {
    case 0:
        vertex.pos = {0.f, 0.f, 0.f};
        break;

    case 1:
        vertex.pos = {size.x, 0.f, 0.f};
        break;

    case 2:
        vertex.pos = {size.x, size.y, 0.f};
        break;

    case 3:
        vertex.pos = {0.f, size.y, 0.f};
        break;
    }

    if (hasGradient) { vertex.color = colorOverrides[i]; }
}

} // namespace gfx
} // namespace bl
