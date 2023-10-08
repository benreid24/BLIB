#include <BLIB/Graphics/Shape2D.hpp>

namespace bl
{
namespace gfx
{
namespace
{
glm::vec3 computeIntersect(const glm::vec3& p1, const glm::vec3& d1, const glm::vec3& p2,
                           const glm::vec3& d2) {
    const float b = (d1.x * (p2.y - p1.y) - d1.y * (p2.x - p1.x)) / (d1.y * d2.x - d1.x * d2.y);
    return {p2.x + d2.x * b, p2.y + d2.y * b, 0.f};
}
} // namespace

Shape2D::Shape2D()
: Drawable()
, OverlayScalable()
, fillColor(1.f, 0.f, 0.f, 1.f)
, outlineColor(0.f, 0.f, 0.f, 1.f)
, outlineThickness(0.f)
, dirty(true)
, updateQueued(false) {}

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

glm::vec4 Shape2D::getCenterColor(const glm::vec4& avgColor) const { return avgColor; }

void Shape2D::markDirty() {
    dirty = true;
    if (!updateQueued && entity() != ecs::InvalidEntity) {
        updateQueued = true;
        engine().systems().addFrameTask(engine::FrameStage::RenderObjectSync,
                                        std::bind(&Shape2D::update, this));
    }
}

void Shape2D::create(engine::Engine& engine) {
    Drawable::create(engine);
    OverlayScalable::create(engine, entity());
    markDirty();
}

void Shape2D::update() {
    if (entity() == ecs::InvalidEntity || !dirty) { return; }
    dirty        = false;
    updateQueued = false;

    // determine required vertex and index counts
    const bool hasOutline           = outlineThickness != 0.f;
    const std::uint32_t pointCount  = getVertexCount();
    const std::uint32_t vertexCount = pointCount * (hasOutline ? 3 : 1) + 1;
    const std::uint32_t indexCount  = pointCount * (hasOutline ? 5 : 3)   // shape points
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
    ib.vertices()[0].color = getCenterColor(avgColor);

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
        const unsigned int j = (i - 1) * 3;
        ib.indices()[j + 0]  = 0;
        ib.indices()[j + 1]  = i;
        ib.indices()[j + 2]  = (i == outlineStartIndex - 1) ? 1 : i + 1;
    }

    if (hasOutline) {
        // populate outline vertices
        const glm::vec3& centerPos = ib.vertices()[0].pos;
        for (unsigned int i = 0; i < pointCount; ++i) {
            auto& inner = ib.vertices()[i + pointCount + 1];
            auto& outer = ib.vertices()[i + pointCount * 2 + 1];

            inner.color = outlineColor;
            outer.color = outlineColor;
            inner.pos   = ib.vertices()[i + 1].pos; // same pos as shape corner

            const auto computeExtrudedCenterPoint =
                [this, &centerPos](const glm::vec3& cur, const glm::vec3& cmp) -> glm::vec3 {
                const glm::vec3 center = (cur + cmp) / 2.f;
                const glm::vec3 dir    = center - centerPos;
                const float len        = glm::length(dir);
                return centerPos + dir * ((len + outlineThickness) / len);
            };

            const glm::vec3& prevPoint = ib.vertices()[i == 0 ? pointCount : i].pos;
            const glm::vec3& curPoint  = ib.vertices()[i + 1].pos;
            const glm::vec3& nextPoint = ib.vertices()[i == pointCount - 1 ? 1 : i + 2].pos;
            const glm::vec3 prevCenter = computeExtrudedCenterPoint(curPoint, prevPoint);
            const glm::vec3 nextCenter = computeExtrudedCenterPoint(curPoint, nextPoint);
            const glm::vec3 prevDir    = prevPoint - curPoint;
            const glm::vec3 nextDir    = nextPoint - curPoint;
            outer.pos                  = computeIntersect(prevCenter, prevDir, nextCenter, nextDir);
        }

        // populate outline indices
        unsigned int ii = pointCount * 3;
        for (unsigned int i = 0; i < pointCount; ++i) {
            const unsigned int j  = i + pointCount + 1;
            const unsigned int k  = i + pointCount * 2 + 1;
            const unsigned int jn = (i == pointCount - 1) ? pointCount + 1 : j + 1;
            const unsigned int kn = (i == pointCount - 1) ? pointCount * 2 + 1 : k + 1;

            ib.indices()[ii + 0] = k;
            ib.indices()[ii + 1] = j;
            ib.indices()[ii + 2] = jn;

            ib.indices()[ii + 3] = k;
            ib.indices()[ii + 4] = jn;
            ib.indices()[ii + 5] = kn;

            ii += 6;
        }
    }

    component().commit();
}

} // namespace gfx
} // namespace bl
