#ifndef BLIB_GRAPHICS_RECTANGLE_HPP
#define BLIB_GRAPHICS_RECTANGLE_HPP

#include <BLIB/Graphics/Shape2D.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Renderable 2d rectangle shape
 *
 * @ingroup Graphics
 */
class Rectangle : public Shape2D {
public:
    /**
     * @brief Initializes the object
     */
    Rectangle();

    /**
     * @brief Creates the rectangle entity and components
     *
     * @param engine The game engine instance
     * @param size The local size of the rectangle in world units
     */
    void create(engine::Engine& engine, const glm::vec2& size);

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
    void setColorGradient(const glm::vec4& topLeft, const glm::vec4& topRight,
                          const glm::vec4& bottomRight, const glm::vec4& bottomLeft);

    /**
     * @brief Overrides the fill color and creates a color gradient instead
     *
     * @param leftColor The color of the left edge
     * @param rightColor The color of the right edge
     */
    void setHorizontalColorGradient(const glm::vec4& leftColor, const glm::vec4& rightColor);

    /**
     * @brief Overrides the fill color and creates a color gradient instead
     *
     * @param topColor The color of the top edge
     * @param bottomColor The color of the bottom edge
     */
    void setVerticalColorGradient(const glm::vec4& topColor, const glm::vec4& bottomColor);

    /**
     * @brief Removes the color gradient and calls back to the fill color
     */
    void removeColorGradient();

private:
    glm::vec2 size;
    glm::vec4 colorOverrides[4];
    bool hasGradient;

    virtual unsigned int getVertexCount() const override;
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) override;
};

} // namespace gfx
} // namespace bl

#endif
