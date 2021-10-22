#ifndef BLIB_GUI_ELEMENTS_TOGGLEBUTTON_HPP
#define BLIB_GUI_ELEMENTS_TOGGLEBUTTON_HPP

#include <BLIB/Interfaces/GUI/Elements/CompositeElement.hpp>
#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Base class for toggleable buttons. RadioButton and CheckBox are derived from this.
 *        Cannot be instantiated
 *
 * @ingroup GUI
 *
 */
class ToggleButton : public CompositeElement<3> {
public:
    virtual ~ToggleButton() = default;

    /**
     * @brief Returns the canvas that should be rendered for the current button state
     *
     */
    const Canvas::Ptr& getVisibleButton() const;

    /**
     * @brief Returns the canvas that should be not rendered
     *
     */
    const Canvas::Ptr& getHiddenButton() const;

    /**
     * @brief Returns the current state of the toggle
     *
     */
    bool getValue() const;

    /**
     * @brief Set the state of the toggle
     *
     */
    void setValue(bool value);

    /**
     * @brief Set the size of the actual toggle button area. Default is (10,10)
     *
     */
    void setToggleButtonSize(const sf::Vector2f& size);

protected:
    /**
     * @brief Create a new ToggleButton
     *
     * @param child The element to put to the right of the button
     */
    ToggleButton(const Element::Ptr& child);

    /**
     * @brief Passes the event to the child then returns false always
     *
     * @param event The event that fired
     * @return False so that the button always generates events
     */
    virtual bool propagateEvent(const Event& event) override;

    /**
     * @brief Returns the size required to display the full button text
     *
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Derived buttons should call the apropriate render methods in renderer. This is
     *        called on button resize and on the first time being rendered
     *
     * @param activeBut
     * @param inactiveBut
     * @param renderer
     */
    virtual void renderToggles(Canvas& activeBut, Canvas& inactiveBut,
                               const Renderer& renderer) const = 0;

    /**
     * @brief Renders the proper button and text/child
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

    /**
     * @brief Handler for clicks. Default behavior toggles the value
     *
     */
    virtual void onClick();

private:
    mutable bool butsRendered;
    Element::Ptr child;
    Canvas::Ptr activeButton;
    Canvas::Ptr inactiveButton;
    bool value;

    virtual void onAcquisition() override;
};

} // namespace gui
} // namespace bl

#endif
