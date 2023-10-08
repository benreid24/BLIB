#ifndef BLIB_GRAPHICS_CIRCLE_HPP
#define BLIB_GRAPHICS_CIRCLE_HPP

#include <BLIB/Graphics/Shape2D.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic circle that can be rendered in scenes
 *
 * @ingroup Graphics
 */
class Circle : public Shape2D {
public:
    /**
     * @brief Creates the circle with the given point count
     *
     * @param pointCount The number of triangles to render with
     */
    Circle(unsigned int pointCount = 300);

    /**
     * @brief Creates the circle entity and components
     *
     * @param engine The game engine instance
     * @param radius The circle radius in local units
     */
    void create(engine::Engine& engine, float radius);

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

} // namespace gfx
} // namespace bl

#endif
