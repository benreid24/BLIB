#ifndef BLIB_OBJECTS_CIRCLE_HPP
#define BLIB_OBJECTS_CIRCLE_HPP

#include <BLIB/Objects/Shapes2D/CircleBase.hpp>
#include <BLIB/Objects/Shapes2D/SingleShape2D.hpp>

namespace bl
{
namespace obj
{
/**
 * @brief Basic circle that can be rendered in scenes
 *
 * @ingroup Objects
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

} // namespace obj
} // namespace bl

#endif
