#include <BLIB/Graphics/Shape2D.hpp>

namespace bl
{
namespace gfx
{
Shape2D::Shape2D()
: Drawable()
, OverlayScalable()
, fillColor(1.f, 0.f, 0.f, 1.f)
, outlineColor(0.f, 0.f, 0.f, 1.f)
, outlineThickness(0.f)
, dirty(true) {}

void Shape2D::setFillColor(const glm::vec4& fc) {
    fillColor = fc;
    markDirty();
}

const glm::vec4& Shape2D::getFillColor() const { return fillColor; }

void Shape2D::setOutlineColor(const glm::vec4& oc) {
    outlineColor = oc;
    markDirty();
}

const glm::vec4& Shape2D::getOutlineColor() const { return outlineColor; }

void Shape2D::setOutlineThickness(float t) {
    outlineThickness = t;
    markDirty();
}

float Shape2D::getOutlineThickness() const { return outlineThickness; }

const sf::FloatRect& Shape2D::getLocalBounds() {
    update();
    return localBounds;
}

void Shape2D::markDirty() {
    dirty = true;
    // TODO - one time run system for tasks to register. runs at certain (arbitrary) frame stage
    update();
}

void Shape2D::create(engine::Engine& engine) {
    Drawable::create(engine);
    OverlayScalable::create(engine, entity());
    markDirty();
}

void Shape2D::update() {
    if (entity() == ecs::InvalidEntity || !dirty) { return; }
    dirty = false;

    // determine required vertex and index counts
    const bool hasOutline           = outlineThickness != 0.f;
    const std::uint32_t pointCount  = getVertexCount();
    const std::uint32_t vertexCount = pointCount * (hasOutline ? 2 : 1) + 1;
    const std::uint32_t indexCount  = pointCount                          // center point
                                     + pointCount * (hasOutline ? 5 : 3)  // shape points
                                     + (hasOutline ? pointCount * 4 : 0); // outline points
    const std::uint32_t outlineStartIndex = pointCount + 1;

    // create the index buffer
    auto& ib = component().indexBuffer;
    if (ib.indexCount() < indexCount || ib.vertexCount() < vertexCount) {
        ib.create(engine().renderer().vulkanState(), vertexCount, indexCount);
    }

    // set colors for all points
    for (unsigned int i = 0; i < outlineStartIndex; ++i) { ib.vertices()[i].color = fillColor; }
    if (hasOutline) {
        for (unsigned int i = outlineStartIndex; i < vertexCount; ++i) {
            ib.vertices()[i].color = outlineColor;
        }
    }

    // populate shape vertices
    for (unsigned int i = 1; i < outlineStartIndex; ++i) {
        populateVertex(i - 1, ib.vertices()[i]);
    }

    // set center vertex color to average color of all vertices
    glm::vec4 avgColor{};
    for (unsigned int i = 1; i < outlineStartIndex; ++i) { avgColor += ib.vertices()[i].color; }
    avgColor /= static_cast<float>(pointCount);
    ib.vertices()[0].color = avgColor;

    // determine bounds and set center vertex
    using FL    = std::numeric_limits<float>;
    localBounds = {FL::max(), FL::max(), FL::min(), FL::min()};
    for (unsigned int i = 1; i < outlineStartIndex; ++i) {
        localBounds.left   = std::min(localBounds.left, ib.vertices()[i].pos.x);
        localBounds.top    = std::min(localBounds.top, ib.vertices()[i].pos.y);
        localBounds.width  = std::max(localBounds.width, ib.vertices()[i].pos.x);
        localBounds.height = std::max(localBounds.height, ib.vertices()[i].pos.y);
    }
    localBounds.width -= localBounds.left;
    localBounds.height -= localBounds.top;
    ib.vertices()[0].pos.x = localBounds.left + localBounds.width * 0.5f;
    ib.vertices()[0].pos.y = localBounds.top + localBounds.height * 0.5f;

    // populate indices for shape vertices
    for (unsigned int i = 1; i < outlineStartIndex; ++i) {
        const unsigned int j = i * 3;
        ib.indices()[j + 0]  = 0;
        ib.indices()[j + 1]  = i;
        ib.indices()[j + 2]  = (i == outlineStartIndex - 1) ? 1 : i + 1;
    }

    // TODO - populate outline vertices

    component().commit();
}

} // namespace gfx
} // namespace bl
