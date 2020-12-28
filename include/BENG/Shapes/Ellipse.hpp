#ifndef BLIB_SHAPES_ELLIPSE_HPP
#define BLIB_SHAPES_ELLIPSE_HPP

#include <SFML/Graphics/Shape.hpp>

namespace bg
{
/**
 * @brief Ellipse shape defined by it's major and minor axes. The elipse is horizontal
 *
 * @ingroup Shapes
 *
 */
class Ellipse : public sf::Shape {
public:
    /**
     * @brief Construct a new Ellipse object
     *
     * @param majorAxis Length of the major axis
     * @param minorAxis Length of the minor axis
     * @param centerOrigin True to center the elipse on it's origin, false for (0,0)
     * @param pointCount Number of points to use to render the shape
     */
    Ellipse(float majorAxis, float minorAxis, bool centerOrigin = true,
            unsigned int pointCount = 60);

    /**
     * @brief Update the major and minor axes
     *
     * @param majorAxis Length of the major axis
     * @param minorAxis Length of the minor axis
     * @param centerOrigin True to center the elipse on it's origin
     */
    void setAxes(float majorAxis, float minorAxis, bool centerOrigin = true);

    /**
     * @brief Get the major axis
     *
     */
    float getMajorAxis() const;

    /**
     * @brief Get the minor axis
     *
     */
    float getMinorAxis() const;

    /**
     * @brief Returns the number of points
     *
     */
    virtual std::size_t getPointCount() const override;

    /**
     * @brief Get the point at the index
     *
     */
    sf::Vector2f getPoint(std::size_t index) const override;

private:
    const unsigned int nPoints;
    float majorRadius;
    float minorRadius;
};
} // namespace bg

#endif