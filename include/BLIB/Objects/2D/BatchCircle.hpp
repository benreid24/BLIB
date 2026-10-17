#ifndef BLIB_OBJECTS_2D_BATCHCIRCLE_HPP
#define BLIB_OBJECTS_2D_BATCHCIRCLE_HPP

#include <BLIB/Objects/2D/Shapes/BatchShape.hpp>
#include <BLIB/Objects/2D/Shapes/CircleBase.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Basic circle that can be rendered in scenes as part of shape batches
 *
 * @ingroup Objects
 */
class BatchCircle : public s2d::CircleBase<s2d::BatchShape> {
public:
    /**
     * @brief Creates the circle with the given point count
     *
     * @param pointCount The number of triangles to render with
     */
    BatchCircle(unsigned int pointCount = 300);

    /**
     * @brief Creates the circle entity and components
     *
     * @param engine The game engine instance
     * @param owner The set of batched shapes to be a part of
     * @param radius The circle radius in local units
     */
    void create(engine::Engine& engine, BatchedShapes& owner, float radius);
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
