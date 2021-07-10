#ifndef BLIB_GUI_ELEMENTS_TOGGLEBUTTON_HPP
#define BLIB_GUI_ELEMENTS_TOGGLEBUTTON_HPP

#include <BLIB/Interfaces/GUI/Elements/Button.hpp>

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
class ToggleButton : public Button {
public:
    virtual ~ToggleButton() = default;

    /**
     * @brief Returns the canvas that should be rendered for the current button state
     *
     */
    Canvas::Ptr getVisibleButton() const;

    /**
     * @brief Returns the canvas that should be not rendered
     *
     */
    Canvas::Ptr getHiddenButton() const;

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
    void setToggleButtonSize(const sf::Vector2i& size);

protected:
    /**
     * @brief Create a new ToggleButton
     *
     * @param child The element to put to the right of the button
     * @param group The group of the Button
     * @param id The id of this button
     */
    ToggleButton(Element::Ptr child, const std::string& group, const std::string& id);

    /**
     * @brief Passes the event to the child then returns false always
     *
     * @param event The event that fired
     * @return False so that the button always generates events
     */
    virtual bool handleRawEvent(const RawEvent& event) override;

    /**
     * @brief Returns the size required to display the full button text
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Packs the buttons and child element
     *
     */
    virtual void onAcquisition() override;

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
     * @brief This must be called in derived elements create method() before returning
     *
     */
    void finishCreate();

    /**
     * @brief Handler for clicks. Default behavior toggles the value
     *
     */
    virtual void onClick();

private:
    mutable bool butsRendered;
    Canvas::Ptr activeButton;
    Canvas::Ptr inactiveButton;
    bool value;

    void updateButtons();
};

} // namespace gui
} // namespace bl

#endif