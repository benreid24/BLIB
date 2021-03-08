#ifndef BLIB_SHAPES_TRIANGLE_HPP
#define BLIB_SHAPES_TRIANGLE_HPP

#include <SFML/Graphics/Shape.hpp>

namespace bl
{
namespace shapes
{
/**
 * @brief Basic triangle comprised of 3 points (which must not be colinear)
 *
 * @ingroup Shapes
 *
 */
class Triangle : public sf::Shape {
public:
    /**
     * @brief Construct a new Triangle object from 3 points
     *
     * @param p1 First point
     * @param p2 Second point
     * @param p3 Third point
     * @param centerOrigin True to set the origin to the center of the triangle
     */
    Triangle(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3,
             bool centerOrigin = true);

    /**
     * @brief Update the points the triangle is made of
     *
     * @param p1 First point
     * @param p2 Second point
     * @param p3 Third point
     * @param centerOrigin True to set the origin to the center of the triangle
     */
    void setPoints(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3,
                   bool centerOrigin = true);

    /**
     * @brief Returns 3
     *
     */
    virtual std::size_t getPointCount() const override;

    /**
     * @brief Returns the point at the given index. No bounds check is performed
     *
     */
    virtual sf::Vector2f getPoint(std::size_t index) const override;

private:
    sf::Vector2f points[3];
};

} // namespace shapes
} // namespace bl

#endif
