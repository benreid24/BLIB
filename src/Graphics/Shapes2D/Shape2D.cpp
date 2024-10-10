#include <BLIB/Graphics/Shapes2D/Shape2D.hpp>

namespace bl
{
namespace gfx
{
namespace s2d
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
: fillColor(1.f, 0.f, 0.f, 1.f)
, outlineColor(0.f, 0.f, 0.f, 1.f)
, outlineThickness(0.f) {}

void Shape2D::setFillColor(const rc::Color& fc) {
    fillColor = fc;
    notifyDirty();
}

const rc::Color& Shape2D::getFillColor() const { return fillColor; }

void Shape2D::setOutlineColor(const rc::Color& oc) {
    outlineColor = oc;
    notifyDirty();
}

const rc::Color& Shape2D::getOutlineColor() const { return outlineColor; }

void Shape2D::setOutlineThickness(float t) {
    outlineThickness = t;
    notifyDirty();
}

float Shape2D::getOutlineThickness() const { return outlineThickness; }

const sf::FloatRect& Shape2D::getLocalBounds() {
    ensureUpdated();
    return localBounds;
}

rc::Color Shape2D::getCenterColor(const rc::Color& avgColor) const { return avgColor; }

std::uint32_t Shape2D::calculateRequiredVertices() const {
    const std::uint32_t pointCount = getVertexCount();
    const bool hasOutline          = outlineThickness != 0.f;
    return pointCount * (hasOutline ? 3 : 1) + 1;
}

std::uint32_t Shape2D::calculateRequiredIndices() const {
    const bool hasOutline          = outlineThickness != 0.f;
    const std::uint32_t pointCount = getVertexCount();
    return pointCount * (hasOutline ? 5 : 3)    // shape points
           + (hasOutline ? pointCount * 4 : 0); // outline points
}

void Shape2D::update(rc::prim::Vertex* vertices, std::uint32_t* indices) {
    // determine required vertex and index counts
    const bool hasOutline                 = outlineThickness != 0.f;
    const std::uint32_t pointCount        = getVertexCount();
    const std::uint32_t vertexCount       = calculateRequiredVertices();
    const std::uint32_t outlineStartIndex = pointCount + 1;

    // set colors for all points
    for (unsigned int i = 0; i < outlineStartIndex; ++i) { vertices[i].color = fillColor; }
    if (hasOutline) {
        for (unsigned int i = outlineStartIndex; i < vertexCount; ++i) {
            vertices[i].color = outlineColor;
        }
    }

    // populate shape vertices
    for (unsigned int i = 1; i < outlineStartIndex; ++i) { populateVertex(i - 1, vertices[i]); }

    // set center vertex color to average color of all vertices
    glm::vec4 avgColor{};
    for (unsigned int i = 1; i < outlineStartIndex; ++i) { avgColor += vertices[i].color; }
    avgColor /= static_cast<float>(pointCount);
    vertices[0].color = getCenterColor(avgColor);

    // determine bounds and set center vertex
    using FL    = std::numeric_limits<float>;
    localBounds = {FL::max(), FL::max(), FL::min(), FL::min()};
    for (unsigned int i = 1; i < outlineStartIndex; ++i) {
        localBounds.left   = std::min(localBounds.left, vertices[i].pos.x);
        localBounds.top    = std::min(localBounds.top, vertices[i].pos.y);
        localBounds.width  = std::max(localBounds.width, vertices[i].pos.x);
        localBounds.height = std::max(localBounds.height, vertices[i].pos.y);
    }
    localBounds.width -= localBounds.left;
    localBounds.height -= localBounds.top;
    vertices[0].pos.x = localBounds.left + localBounds.width * 0.5f;
    vertices[0].pos.y = localBounds.top + localBounds.height * 0.5f;

    // populate indices for shape vertices
    for (unsigned int i = 1; i < outlineStartIndex; ++i) {
        const unsigned int j = (i - 1) * 3;
        indices[j + 0]       = 0;
        indices[j + 1]       = i;
        indices[j + 2]       = (i == outlineStartIndex - 1) ? 1 : i + 1;
    }

    if (hasOutline) {
        // populate outline vertices
        const glm::vec3& centerPos = vertices[0].pos;
        for (unsigned int i = 0; i < pointCount; ++i) {
            auto& inner = vertices[i + pointCount + 1];
            auto& outer = vertices[i + pointCount * 2 + 1];

            inner.color = outlineColor;
            outer.color = outlineColor;
            inner.pos   = vertices[i + 1].pos; // same pos as shape corner

            const auto computeExtrudedCenterPoint =
                [this, &centerPos](const glm::vec3& cur, const glm::vec3& cmp) -> glm::vec3 {
                const glm::vec3 center = (cur + cmp) / 2.f;
                const glm::vec3 dir    = center - centerPos;
                const float len        = glm::length(dir);
                return centerPos + dir * ((len + outlineThickness) / len);
            };

            const glm::vec3& prevPoint = vertices[i == 0 ? pointCount : i].pos;
            const glm::vec3& curPoint  = vertices[i + 1].pos;
            const glm::vec3& nextPoint = vertices[i == pointCount - 1 ? 1 : i + 2].pos;
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

            indices[ii + 0] = k;
            indices[ii + 1] = j;
            indices[ii + 2] = jn;

            indices[ii + 3] = k;
            indices[ii + 4] = jn;
            indices[ii + 5] = kn;

            ii += 6;
        }

        // re-determine local bounds with outline accounted for
        localBounds = {FL::max(), FL::max(), FL::min(), FL::min()};
        for (unsigned int i = 0; i < vertexCount; ++i) {
            const auto& v      = vertices[i];
            localBounds.left   = std::min(localBounds.left, v.pos.x);
            localBounds.top    = std::min(localBounds.top, v.pos.y);
            localBounds.width  = std::max(localBounds.width, v.pos.x);
            localBounds.height = std::max(localBounds.height, v.pos.y);
        }
        localBounds.width -= localBounds.left;
        localBounds.height -= localBounds.top;
    }

    // TODO - call into storage layer? no, just remember to refresh local bounds and commit
}

} // namespace s2d
} // namespace gfx
} // namespace bl
