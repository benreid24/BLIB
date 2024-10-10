#ifndef BLIB_GRAPHICS_SHAPES2D_TRIANGLEBASE_HPP
#define BLIB_GRAPHICS_SHAPES2D_TRIANGLEBASE_HPP

#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>
#include <array>
#include <type_traits>

namespace bl
{
namespace gfx
{
namespace s2d
{
/**
 * @brief Intermediate base class for triangles. Use gfx::TriangleBase or BatchedTriangle
 *
 * @tparam B Base class to use
 * @ingroup Graphics
 */
template<typename B>
class TriangleBase : public B {
    static_assert(std::is_base_of_v<Shape2D, B>, "B must derive from Shape2D");

public:
    /**
     * @brief Does nothing
     */
    TriangleBase();

    /**
     * @brief Sets the points to make the triangle with
     *
     * @param p0 The first point
     * @param p1 The second point
     * @param p2 The third point
     */
    void setPoints(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2);

    /**
     * @brief Sets the points to make the triangle with
     *
     * @param points The points to render
     */
    void setPoints(const std::array<glm::vec2, 3>& points);

    /**
     * @brief Overrides the fill color to create a gradient with each point having it's own color
     *
     * @param c0 The color of the first point
     * @param c1 The color of the second point
     * @param c2 The color of the third point
     */
    void setColorGradient(const rc::Color& c0, const rc::Color& c1, const rc::Color& c2);

    /**
     * @brief Removes the color gradient and falls back to the fill color
     */
    void removeColorGradient();

private:
    glm::vec2 points[3];
    rc::Color colors[3];
    bool hasColorOverride;

    virtual unsigned int getVertexCount() const override;
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename B>
TriangleBase<B>::TriangleBase()
: B()
, points{}
, colors{}
, hasColorOverride(false) {}

template<typename B>
void TriangleBase<B>::setPoints(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2) {
    points[0] = p0;
    points[1] = p1;
    points[2] = p2;
    B::notifyDirty();
}

template<typename B>
void TriangleBase<B>::setPoints(const std::array<glm::vec2, 3>& p) {
    points[0] = p[0];
    points[1] = p[1];
    points[2] = p[2];
    B::notifyDirty();
}

template<typename B>
void TriangleBase<B>::setColorGradient(const rc::Color& c0, const rc::Color& c1,
                                       const rc::Color& c2) {
    colors[0]        = c0;
    colors[1]        = c1;
    colors[2]        = c2;
    hasColorOverride = true;
    B::notifyDirty();
}

template<typename B>
void TriangleBase<B>::removeColorGradient() {
    hasColorOverride = false;
    B::notifyDirty();
}

template<typename B>
unsigned int TriangleBase<B>::getVertexCount() const {
    return 3;
}

template<typename B>
void TriangleBase<B>::populateVertex(unsigned int i, rc::prim::Vertex& vertex) {
    vertex.pos = {points[i], 0.f};
    if (hasColorOverride) { vertex.color = colors[i]; }
}

} // namespace s2d
} // namespace gfx
} // namespace bl

#endif
