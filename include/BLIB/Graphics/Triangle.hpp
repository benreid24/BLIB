#ifndef BLIB_GRAPHICS_TRIANGLE_HPP
#define BLIB_GRAPHICS_TRIANGLE_HPP

#include <BLIB/Graphics/Shape2D.hpp>
#include <array>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic renderable triangle that can be added to scenes
 *
 * @ingroup Graphics
 */
class Triangle : public Shape2D {
public:
    /**
     * @brief Does nothing
     */
    Triangle();

    /**
     * @brief Creates the triangle shape from 3 points (should not be co-linear)
     *
     * @param engine The game engine instance
     * @param p0 The first point
     * @param p1 The second point
     * @param p2 The third point
     */
    void create(engine::Engine& engine, const glm::vec2& p0, const glm::vec2& p1,
                const glm::vec2& p2);

    /**
     * @brief Creates the triangle shape from 3 points (should not be co-linear)
     *
     * @param engine The game engine instance
     * @param points The 3 points to create the triangle from
     */
    void create(engine::Engine& engine, const std::array<glm::vec2, 3>& points);

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
    void setColorGradient(const glm::vec4& c0, const glm::vec4& c1, const glm::vec4& c2);

    /**
     * @brief Removes the color gradient and falls back to the fill color
     */
    void removeColorGradient();

private:
    glm::vec2 points[3];
    glm::vec4 colors[3];
    bool hasColorOverride;

    virtual unsigned int getVertexCount() const override;
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) override;
};

} // namespace gfx
} // namespace bl

#endif
