#ifndef BLIB_OBJECTS_BATCHRECTANGLE_HPP
#define BLIB_OBJECTS_BATCHRECTANGLE_HPP

#include <BLIB/Objects/Shapes2D/BatchShape2D.hpp>
#include <BLIB/Objects/Shapes2D/RectangleBase.hpp>

namespace bl
{
namespace obj
{
/**
 * @brief Renderable 2d rectangle shape for use in batched shapes
 *
 * @ingroup Objects
 */
class BatchRectangle : public s2d::RectangleBase<s2d::BatchShape2D> {
public:
    /**
     * @brief Initializes the object
     */
    BatchRectangle();

    /**
     * @brief Creates the rectangle
     *
     * @param engine The game engine instance
     * @param owner The set of batched shapes to be a part of
     * @param size The local size of the rectangle in world units
     */
    void create(engine::Engine& engine, BatchedShapes2D& owner, const glm::vec2& size);
};

} // namespace obj
} // namespace bl

#endif
