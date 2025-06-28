#ifndef BLIB_COMPONENTS_OUTLINE_HPP
#define BLIB_COMPONENTS_OUTLINE_HPP

#include <BLIB/Render/Color.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component that adds an outline to 3d objects when rendered
 *
 * @ingroup Components
 */
class Outline {
public:
    /**
     * @brief Creates the outline component
     *
     * @param color The color of the outline
     * @param thickness The thickness of the outline in world units
     */
    Outline(const rc::Color& color, float thickness)
    : color(color)
    , thickness(thickness) {}

    /**
     * @brief Copies the outline
     */
    Outline(const Outline&) = default;

    /**
     * @brief Copies the outline
     */
    Outline& operator=(const Outline&) = default;

    /**
     * @brief Sets the color of the outline
     *
     * @param c The new color of the outline
     */
    void setColor(const rc::Color& c) { color = c; }

    /**
     * @brief Returns the color of the outline
     */
    const rc::Color& getColor() const { return color; }

    /**
     * @brief Returns the thickness of the outline
     */
    float getThickness() const { return thickness; }

    /**
     * @brief Sets the thickness of the outline
     *
     * @param s The new thickness in world units
     */
    void setThickness(float s) { thickness = s; }

private:
    rc::Color color;
    float thickness;
};

} // namespace com
} // namespace bl

#endif
