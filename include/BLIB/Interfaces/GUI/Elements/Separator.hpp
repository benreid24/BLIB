#ifndef BLIB_GUI_ELEMENTS_SEPARATOR_HPP
#define BLIB_GUI_ELEMENTS_SEPARATOR_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
/***
 * @brief Basic element that is represented as a line between other elements. Note that the
 *        outline thickness is used as the line thickness and fill color for the line color
 *
 * @ingroup GUI
 */
class Separator : public Element {
public:
    typedef std::shared_ptr<Separator> Ptr;

    static constexpr float DefaultThickness = 1.f;

    virtual ~Separator() = default;

    /// Direction the separator should be in
    enum Direction { Horizontal, Vertical };

    /**
     * @brief Create a new separator in the given direction
     *
     * @param direction The direction to orient the separator
     * @return Ptr The new separator
     */
    static Ptr create(Direction direction);

    /**
     * @brief Returns the direction the separator is facing
     *
     */
    Direction getDirection() const;

    /**
     * @brief Convenience method to get the thickness of the separator
     */
    float getThickness() const {
        return renderSettings().outlineThickness.value_or(DefaultThickness);
    }

protected:
    /**
     * @brief Create a new separator in the given direction
     *
     * @param direction The direction to orient the separator
     */
    Separator(Direction direction);

    /**
     * @brief Returns the outline thickness in both the x and y components
     *
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Creates the visual component for this element
     *
     * @param renderer The renderer to use to create visual Components
     * @return The visual component for this element
     */
    virtual rdr::Component* doPrepareRender(rdr::Renderer& renderer) override;

private:
    const Direction dir;
};

} // namespace gui
} // namespace bl

#endif
