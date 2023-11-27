#ifndef BLIB_GUI_ELEMENTS_TOGGLEBUTTON_HPP
#define BLIB_GUI_ELEMENTS_TOGGLEBUTTON_HPP

#include <BLIB/Interfaces/GUI/Elements/CompositeElement.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Base class for toggle-able buttons. RadioButton and CheckBox are derived from this.
 *        Cannot be instantiated
 *
 * @ingroup GUI
 */
class ToggleButton : public CompositeElement<1> {
public:
    /**
     * @brief Destroys the toggle button
     */
    virtual ~ToggleButton() = default;

    /**
     * @brief Returns the current state of the toggle
     */
    bool getValue() const;

    /**
     * @brief Set the state of the toggle
     */
    void setValue(bool value);

    /**
     * @brief Sets the size, in overlay coordinates, of the toggle indicator
     *
     * @param size The size of the indicator (square edge, circle diameter, etc)
     */
    void setToggleSize(float size);

    /**
     * @brief Returns the size of the toggle indicator in overlay coordinates
     */
    float getToggleSize() const;

protected:
    /**
     * @brief Create a new ToggleButton
     *
     * @param child The element to put to the right of the button
     */
    ToggleButton(const Element::Ptr& child);

    /**
     * @brief Returns the size required to display the full button text
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Handler for clicks. Default behavior toggles the value
     */
    virtual void onClick();

    /**
     * @brief Passes the event to the child then returns false always
     *
     * @param event The event that fired
     * @return False so that the button always generates events
     */
    virtual bool propagateEvent(const Event& event) override;

    /**
     * @brief Called when a new acquisition is assigned
     */
    virtual void onAcquisition() override;

private:
    Element::Ptr child;
    bool value;
    mutable float toggleSize;
};

} // namespace gui
} // namespace bl

#endif
