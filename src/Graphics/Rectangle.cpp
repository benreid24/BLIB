#include <BLIB/Graphics/Rectangle.hpp>

namespace bl
{
namespace gfx
{
Rectangle::Rectangle()
: Shape2D()
, size(50.f, 50.f) {}

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
}

} // namespace gfx
} // namespace bl
