#ifndef BLIB_SHAPES_GRADIENTCIRCLE_HPP
#define BLIB_SHAPES_GRADIENTCIRCLE_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace bg
{
/**
 * @brief A circle, similar to sf::CircleShape, but with a color gradient between the
 *        circumfrence and center
 *
 * @ingroup Shapes
 *
 */
class GradientCircle
: public sf::Transformable
, public sf::Drawable {
public:
    /**
     * @brief Construct a new GradientCircle
     *
     * @param radius The radius of the circle
     * @param centerOrigin True to put the origin at the center
     * @param pointCount Number of points to use to render the circle
     */
    GradientCircle(float radius, bool centerOrigin = true, unsigned int pointCount = 120);

    /**
     * @brief Set the radius
     *
     * @param radius The radius of the circle
     * @param centerOrigin True to put the origin at the center
     */
    void setRadius(float radius, bool centerOrigin = true);

    /**
     * @brief Get the radius
     *
     */
    float getRadius() const;

    /**
     * @brief Set the color at the center of the circle
     *
     * @param color The color to converge to at the center
     */
    void setCenterColor(const sf::Color& color);

    /**
     * @brief Get the center color
     *
     */
    sf::Color getCenterColor() const;

    /**
     * @brief Set the color at the outside of the circle
     *
     * @param color The color to render at the radius
     */
    void setOuterColor(const sf::Color& color);

    /**
     * @brief Get the outer color
     *
     */
    sf::Color getOuterColor() const;

protected:
    /**
     * @brief Renders the circle
     *
     * @param target Target to render to
     * @param states The render states to use
     */
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::VertexArray points;
    float radius;

    void updatePoints();
};
} // namespace bg

#endif