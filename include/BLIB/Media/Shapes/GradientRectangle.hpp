#ifndef BLIB_SHAPES_GRADIENTRECTANGLE_HPP
#define BLIB_SHAPES_GRADIENTRECTANGLE_HPP

#include <BLIB/Media/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

namespace bl
{
namespace shapes
{
/**
 * @brief Drawable rectangle that can have different colors in each corner. Includes some helpers to
 *        make gradient construction less verbose
 *
 * @ingroup Shapes
 *
 */
class GradientRectangle
: public sf::Transformable
, public sf::Drawable {
public:
    /// @brief Represents the direction to fade colors in
    enum Direction { TopToBottom, BottomToTop, LeftToRight, RightToLeft };

    /**
     * @brief Construct a new Gradient Rectangle with the given gradient
     *
     * @param size The size of the rectangle
     * @param gradientDir The direction to fade the colors in
     * @param fromColor The color to start with on the source side
     * @param toColor The color to finish with on the end side
     */
    GradientRectangle(const sf::Vector2f& size, Direction gradientDir, sf::Color fromColor,
                      sf::Color toColor);

    /**
     * @brief Construct a new Gradient Rectangle with the given corner colors
     *
     * @param size The size of the rectangle
     * @param topLeft The color of the top left point
     * @param topRight The color of the top right point
     * @param bottomLeft The color of the bottom left point
     * @param bottomRight The color of the bottom right point
     */
    GradientRectangle(const sf::Vector2f& size, sf::Color topLeft, sf::Color topRight,
                      sf::Color bottomLeft, sf::Color bottomRight);

    /**
     * @brief Set the size of the rectangle
     *
     * @param size The size of the rectangle
     */
    void setSize(const sf::Vector2f& size);

    /**
     * @brief Returns the size of the rectangle
     *
     * @return const sf::Vector2f& The current size of the rectangle
     */
    const sf::Vector2f& getSize() const;

    /**
     * @brief Set the gradient from the direction and colors
     *
     * @param gradientDir The direction to fade the colors in
     * @param fromColor The color to start with on the source side
     * @param toColor The color to finish with on the end side
     */
    void setGradient(Direction gradientDir, sf::Color fromColor, sf::Color toColor);

    /**
     * @brief Set the gradient based on each point separately
     *
     * @param topLeft The color of the top left point
     * @param topRight The color of the top right point
     * @param bottomLeft The color of the bottom left point
     * @param bottomRight The color of the bottom right point
     */
    void setGradient(sf::Color topLeft, sf::Color topRight, sf::Color bottomLeft,
                     sf::Color bottomRight);

protected:
    /**
     * @brief Renders the rectangle to the given target
     *
     * @param target The target to render to
     * @param states The render states to render with
     */
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    gfx::VertexBuffer points;

    GradientRectangle(const sf::Vector2f& size);
};

} // namespace shapes
} // namespace bl

#endif
