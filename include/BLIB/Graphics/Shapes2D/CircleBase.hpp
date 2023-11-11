#ifndef BLIB_GRAPHICS_SHAPES2D_CIRCLEBASE_HPP
#define BLIB_GRAPHICS_SHAPES2D_CIRCLEBASE_HPP

#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>
#include <BLIB/Math.hpp>
#include <cmath>
#include <type_traits>

namespace bl
{
namespace gfx
{
namespace s2d
{
/**
 * @brief Intermediate base class for circles. Use gfx::CircleBase or gfx::BatchedCircle
 *
 * @tparam B The base class to use
 * @ingroup Graphics
 */
template<typename B>
class CircleBase : public B {
    static_assert(std::is_base_of_v<Shape2D, B>, "B must derive from Shape2D");

public:
    /**
     * @brief Creates the circle with the given point count
     *
     * @param pointCount The number of triangles to render with
     */
    CircleBase(unsigned int pointCount = 300);

    /**
     * @brief Sets the circles radius
     *
     * @param radius The radius in local units
     */
    void setRadius(float radius);

    /**
     * @brief Returns the radius of the circle in local units
     */
    float getRadius() const;

    /**
     * @brief Sets the number of triangles to render with. Higher numbers will look smoother at the
     *        cost of performance
     *
     * @param pointCount The number of triangles to render with
     */
    void setPointCount(unsigned int pointCount);

    /**
     * @brief Overrides the fill color and creates a color gradient instead
     *
     * @param centerColor The color of the center of the circle
     * @param edgeColor The color of the outside of the circle
     */
    void setColorGradient(const glm::vec4& centerColor, const glm::vec4& edgeColor);

    /**
     * @brief Removes the color gradient and falls back to the fill color
     */
    void removeColorGradient();

private:
    unsigned int pointCount;
    float radius;
    bool hasColorOverride;
    glm::vec4 centerColor;
    glm::vec4 edgeColor;

    virtual unsigned int getVertexCount() const override;
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) override;
    virtual glm::vec4 getCenterColor(const glm::vec4& avgColor) const override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename B>
CircleBase<B>::CircleBase(unsigned int pointCount)
: B()
, pointCount(pointCount)
, radius(100.f)
, hasColorOverride(false)
, centerColor{}
, edgeColor{} {}

template<typename B>
void CircleBase<B>::setRadius(float r) {
    radius = r;
    B::notifyDirty();
}

template<typename B>
float CircleBase<B>::getRadius() const {
    return radius;
}

template<typename B>
void CircleBase<B>::setPointCount(unsigned int pc) {
    pointCount = pc;
    B::notifyDirty();
}

template<typename B>
void CircleBase<B>::setColorGradient(const glm::vec4& c, const glm::vec4& e) {
    hasColorOverride = true;
    centerColor      = c;
    edgeColor        = e;
    B::notifyDirty();
}

template<typename B>
void CircleBase<B>::removeColorGradient() {
    hasColorOverride = false;
    B::notifyDirty();
}

template<typename B>
unsigned int CircleBase<B>::getVertexCount() const {
    return pointCount + 1; // repeat first point
}

template<typename B>
void CircleBase<B>::populateVertex(unsigned int index, rc::prim::Vertex& vertex) {
    if (hasColorOverride) { vertex.color = edgeColor; }

    const float percent = static_cast<float>(index) / static_cast<float>(pointCount);
    const float radians = percent * 2.f * math::Pi;
    vertex.pos.x        = std::cos(radians) * radius + radius;
    vertex.pos.y        = std::sin(radians) * radius + radius;
    vertex.pos.z        = 0.f;
}

template<typename B>
glm::vec4 CircleBase<B>::getCenterColor(const glm::vec4& avgColor) const {
    return hasColorOverride ? centerColor : avgColor;
}

} // namespace s2d
} // namespace gfx
} // namespace bl

#endif
