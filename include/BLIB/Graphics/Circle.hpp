#ifndef BLIB_GRAPHICS_CIRCLE_HPP
#define BLIB_GRAPHICS_CIRCLE_HPP

#include <BLIB/Graphics/Shapes2D/CircleBase.hpp>
#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic circle that can be rendered in scenes
 *
 * @ingroup Graphics
 */
class Circle : public s2d::CircleBase<s2d::SingleShape2D> {
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
};

} // namespace gfx
} // namespace bl

#endif
