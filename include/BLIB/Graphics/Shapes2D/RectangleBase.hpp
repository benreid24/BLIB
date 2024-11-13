#ifndef BLIB_GRAPHICS_SHAPES2D_RECTANGLEBASE_HPP
#define BLIB_GRAPHICS_SHAPES2D_RECTANGLEBASE_HPP

#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>
#include <type_traits>

namespace bl
{
namespace gfx
{
namespace s2d
{
/**
 * @brief Intermediate base class for rectangles. Use gfx::Rectangle or BatchedRectangle
 *
 * @tparam B The base class to use
 * @ingroup Graphics
 */
template<typename B>
class RectangleBase : public B {
    static_assert(std::is_base_of_v<Shape2D, B>, "B must derive from Shape2D");

public:
    /**
     * @brief Initializes the object
     */
    RectangleBase();

    /**
     * @brief Sets the local size of the rectangle in world units. This is the size prior to the
     *        transform. The vertices of the rectangle are updated
     *
     * @param size The local size of the rectangle in world units
     */
    void setSize(const glm::vec2& size);

    /**
     * @brief Returns the local size of the rectangle
     */
    const glm::vec2& getSize() const;

    /**
     * @brief Overrides the fill color and creates a color gradient instead
     *
     * @param topLeft The color of the top left corner
     * @param topRight The color of the top right corner
     * @param bottomRight The color of the bottom right corner
     * @param bottomLeft The color of the bottom left corner
     */
    void setColorGradient(const rc::Color& topLeft, const rc::Color& topRight,
                          const rc::Color& bottomRight, const rc::Color& bottomLeft);

    /**
     * @brief Overrides the fill color and creates a color gradient instead
     *
     * @param leftColor The color of the left edge
     * @param rightColor The color of the right edge
     */
    void setHorizontalColorGradient(const rc::Color& leftColor, const rc::Color& rightColor);

    /**
     * @brief Overrides the fill color and creates a color gradient instead
     *
     * @param topColor The color of the top edge
     * @param bottomColor The color of the bottom edge
     */
    void setVerticalColorGradient(const rc::Color& topColor, const rc::Color& bottomColor);

    /**
     * @brief Removes the color gradient and calls back to the fill color
     */
    void removeColorGradient();

private:
    glm::vec2 size;
    rc::Color colorOverrides[4];
    bool hasGradient;

    virtual unsigned int getVertexCount() const override;
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename B>
RectangleBase<B>::RectangleBase()
: B()
, size(50.f, 50.f)
, colorOverrides{}
, hasGradient(false) {}

template<typename B>
void RectangleBase<B>::setSize(const glm::vec2& s) {
    size = s;
    B::notifyDirty();
}

template<typename B>
const glm::vec2& RectangleBase<B>::getSize() const {
    return size;
}

template<typename B>
void RectangleBase<B>::setColorGradient(const rc::Color& topLeft, const rc::Color& topRight,
                                        const rc::Color& bottomRight, const rc::Color& bottomLeft) {
    colorOverrides[0] = topLeft;
    colorOverrides[1] = topRight;
    colorOverrides[2] = bottomRight;
    colorOverrides[3] = bottomLeft;
    hasGradient       = true;
    B::notifyDirty();
}

template<typename B>
void RectangleBase<B>::setHorizontalColorGradient(const rc::Color& leftColor,
                                                  const rc::Color& rightColor) {
    colorOverrides[0] = leftColor;
    colorOverrides[1] = rightColor;
    colorOverrides[2] = rightColor;
    colorOverrides[3] = leftColor;
    hasGradient       = true;
    B::notifyDirty();
}

template<typename B>
void RectangleBase<B>::setVerticalColorGradient(const rc::Color& topColor,
                                                const rc::Color& bottomColor) {
    colorOverrides[0] = topColor;
    colorOverrides[1] = topColor;
    colorOverrides[2] = bottomColor;
    colorOverrides[3] = bottomColor;
    hasGradient       = true;
    B::notifyDirty();
}

template<typename B>
void RectangleBase<B>::removeColorGradient() {
    hasGradient = false;
    B::notifyDirty();
}

template<typename B>
unsigned int RectangleBase<B>::getVertexCount() const {
    return 4;
}

template<typename B>
void RectangleBase<B>::populateVertex(unsigned int i, rc::prim::Vertex& vertex) {
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

} // namespace s2d
} // namespace gfx
} // namespace bl

#endif
