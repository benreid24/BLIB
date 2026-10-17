#ifndef BLIB_OBJECTS_2D_RECTANGLE_HPP
#define BLIB_OBJECTS_2D_RECTANGLE_HPP

#include <BLIB/Objects/2D/Shapes/RectangleBase.hpp>
#include <BLIB/Objects/2D/Shapes/SingleShape.hpp>
namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Renderable 2d rectangle shape
 *
 * @ingroup Objects
 */
class Rectangle : public s2d::RectangleBase<s2d::SingleShape> {
public:
    /**
     * @brief Initializes the object
     */
    Rectangle();

    /**
     * @brief Creates the rectangle entity and components
     *
     * @param world The world to create the object in
     * @param size The local size of the rectangle in world units
     */
    void create(engine::World& world, const glm::vec2& size);

    /**
     * @brief Creates the rectangle from existing entity and components
     *
     * @param world The world to create the object in
     * @param existingEntity The existing entity to add components to
     * @param size The local size of the rectangle in world units
     */
    void create(engine::World& world, ecs::Entity existingEntity, const glm::vec2& size);
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
