#ifndef BLIB_COMPONENTS_HITBOX_2D_HPP
#define BLIB_COMPONENTS_HITBOX_2D_HPP

#include <BLIB/Components/Transform2D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace com
{
/**
 * @brief 2D hitbox component that can handle a few different shapes
 *
 * @ingroup Components
 */
class Hitbox2D {
public:
    /// The different shapes the hitbox can be
    enum Type { Point, Circle, Rectangle };

    /**
     * @brief Creates a hitbox that is just a point
     *
     * @param transform The transform component for this entity
     */
    Hitbox2D(Transform2D* transform);

    /**
     * @brief Creates a circular hitbox
     *
     * @param transform The transform component for this entity
     * @param radius The radius of the hitbox
     */
    Hitbox2D(Transform2D* transform, float radius);

    /**
     * @brief Creates a rectangular hitbox
     *
     * @param transform The transform component of this entity
     * @param size The size of the rectangle
     */
    Hitbox2D(Transform2D* transform, const glm::vec2& size);

    /**
     * @brief Returns whether this hitbox intersects with the given hitbox
     *
     * @param other The other hitbox to test
     * @return True if the hitboxes overlap, false otherwise
     */
    bool intersects(const Hitbox2D& other) const;

    /**
     * @brief Returns whether this hitbox contains the given point
     *
     * @param point The point to test in global space
     * @return True if the point is inside the box, false otherwise
     */
    bool containsPoint(const glm::vec2& point) const;

    /**
     * @brief Returns whether this hitbox intersects the given circle
     *
     * @param center The center of the circle to test in global space
     * @param radius The radius of the circle to test
     * @return True if the circle overlaps this hitbox, false otherwise
     */
    bool intersectsCircle(const glm::vec2& center, float radius) const;

    /**
     * @brief Returns whether this hitbox intersects the given rectangle
     *
     * @param corner The upper left corner of the rectangle in global space
     * @param size The size of the rectangle
     * @return True if the rectangle overlaps this hitbox, false otherwise
     */
    bool intersectsRect(const glm::vec2& corner, const glm::vec2& size) const;

    /**
     * @brief Returns what type of hitbox this is
     */
    Type getType() const { return type; }

    /**
     * @brief Returns the radius of this hitbox. Only valid for Circle
     */
    float getRadius() const { return radius; }

    /**
     * @brief Returns the size of this hitbox. Only valid for Rectangle
     */
    const glm::vec2& getSize() const { return size; }

    /**
     * @brief Turns this hitbox into a Point
     */
    void makePoint();

    /**
     * @brief Turns this hitbox into a Circle
     *
     * @param radius The radius of the circle
     */
    void makeCircle(float radius);

    /**
     * @brief Turns this hitbox into a Rectangle
     *
     * @param size The size of the rectangle
     */
    void makeRect(const glm::vec2& size);

private:
    Transform2D* transform;
    Type type;
    union {
        glm::vec2 size;
        float radius;
    };
};

} // namespace com
} // namespace bl

#endif
