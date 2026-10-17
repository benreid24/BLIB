#ifndef BLIB_OBJECTS_2D_BATCHRECTANGLE_HPP
#define BLIB_OBJECTS_2D_BATCHRECTANGLE_HPP

#include <BLIB/Objects/2D/Shapes/BatchShape.hpp>
#include <BLIB/Objects/2D/Shapes/RectangleBase.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Renderable 2d rectangle shape for use in batched shapes
 *
 * @ingroup Objects
 */
class BatchRectangle : public s2d::RectangleBase<s2d::BatchShape> {
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
    void create(engine::Engine& engine, BatchedShapes& owner, const glm::vec2& size);
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
