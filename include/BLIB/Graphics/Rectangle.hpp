#ifndef BLIB_GRAPHICS_RECTANGLE_HPP
#define BLIB_GRAPHICS_RECTANGLE_HPP

#include <BLIB/Graphics/Shapes2D/RectangleBase.hpp>
#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Renderable 2d rectangle shape
 *
 * @ingroup Graphics
 */
class Rectangle : public s2d::RectangleBase<s2d::SingleShape2D> {
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
};

} // namespace gfx
} // namespace bl

#endif
