#ifndef BLIB_OBJECTS_2D_BATCHTRIANGLE_HPP
#define BLIB_OBJECTS_2D_BATCHTRIANGLE_HPP

#include <BLIB/Objects/2D/Shapes/BatchShape.hpp>
#include <BLIB/Objects/2D/Shapes/TriangleBase.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Basic renderable triangle that can be added to scenes as part of shape batches
 *
 * @ingroup Objects
 */
class BatchTriangle : public s2d::TriangleBase<s2d::BatchShape> {
public:
    /**
     * @brief Does nothing
     */
    BatchTriangle();

    /**
     * @brief Creates the triangle shape from 3 points (should not be co-linear)
     *
     * @param engine The game engine instance
     * @param owner The set of batched shapes to be a part of
     * @param p0 The first point
     * @param p1 The second point
     * @param p2 The third point
     */
    void create(engine::Engine& engine, BatchedShapes& owner, const glm::vec2& p0,
                const glm::vec2& p1, const glm::vec2& p2);

    /**
     * @brief Creates the triangle shape from 3 points (should not be co-linear)
     *
     * @param engine The game engine instance
     * @param owner The set of batched shapes to be a part of
     * @param points The 3 points to create the triangle from
     */
    void create(engine::Engine& engine, BatchedShapes& owner,
                const std::array<glm::vec2, 3>& points);
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
