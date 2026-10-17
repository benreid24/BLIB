#ifndef BLIB_OBJECTS_2D_CIRCLE_HPP
#define BLIB_OBJECTS_2D_CIRCLE_HPP

#include <BLIB/Objects/2D/Shapes/CircleBase.hpp>
#include <BLIB/Objects/2D/Shapes/SingleShape.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Basic circle that can be rendered in scenes
 *
 * @ingroup Objects
 */
class Circle : public s2d::CircleBase<s2d::SingleShape> {
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
     * @param world The world to create the object in
     * @param radius The circle radius in local units
     */
    void create(engine::World& world, float radius);

    /**
     * @brief Creates the circle components on an existing entity
     *
     * @param world The world to create the object in
     * @param existingEntity The entity to add the sprite components to
     * @param radius The circle radius in local units
     */
    void create(engine::World& world, ecs::Entity existingEntity, float radius);
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
