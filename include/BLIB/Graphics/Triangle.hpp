#ifndef BLIB_GRAPHICS_TRIANGLE_HPP
#define BLIB_GRAPHICS_TRIANGLE_HPP

#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>
#include <BLIB/Graphics/Shapes2D/TriangleBase.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic renderable triangle that can be added to scenes
 *
 * @ingroup Graphics
 */
class Triangle : public s2d::TriangleBase<s2d::SingleShape2D> {
public:
    /**
     * @brief Does nothing
     */
    Triangle();

    /**
     * @brief Creates the triangle shape from 3 points (should not be co-linear)
     *
     * @param world The world to create the object in
     * @param p0 The first point
     * @param p1 The second point
     * @param p2 The third point
     */
    void create(engine::World& world, const glm::vec2& p0, const glm::vec2& p1,
                const glm::vec2& p2);

    /**
     * @brief Creates the triangle shape from 3 points (should not be co-linear)
     *
     * @param world The world to create the object in
     * @param points The 3 points to create the triangle from
     */
    void create(engine::World& world, const std::array<glm::vec2, 3>& points);
};

} // namespace gfx
} // namespace bl

#endif
