#ifndef BLIB_GUI_ELEMENTS_HPP
#define BLIB_GUI_ELEMENTS_HPP

#include <BLIB/Interfaces/GUI/Elements/CompositeElement.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief A clickable button with a single child element, typically a Label
 *
 * @ingroup GUI
 *
 */
class Button : public CompositeElement<1> {
public:
    typedef std::shared_ptr<Button> Ptr;

    static constexpr float DefaultOutlineThickness = 2.f;

    /**
     * @brief Create a new Button with a label inside of it
     *
     * @param text The text to display inside the button
     * @param group The group of the Button
     * @param id The id of this button
     * @return Ptr Pointer to the new Button
     */
    static Ptr create(const std::string& text);

    /**
     * @brief Create a new Button with any element inside. Typically used for images
     *
     * @param child The child element to put inside the button
     * @param group The group of the Button
     * @param id The id of this button
     * @return Ptr Pointer to the new Button
     */
    static Ptr create(const Element::Ptr& child);

    /**
     * @brief Get the child element of the Button. Typically this is a Label but may be
     *        anything
     *
     * @return Element::Ptr The child element that renders inside the button
     */
    const Element::Ptr& getChild() const;

    /**
     * @brief Sets the padding between the child content and button border. Default is 4
     *
     * @param padding The amount of space between the child element and button border
     * @param markDirty True to mark the button for repacking, false to skip
     */
    void setChildPadding(float padding, bool markDirty = true);

protected:
    /**
     * @brief Create a new Button
     *
     * @param c The element to put inside the button
     */
    Button(const Element::Ptr& c);

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
     * @brief Creates the visual component for this element
     *
     * @param renderer The renderer to use to create visual Components
     * @return The visual component for this element
     */
    virtual rdr::Component* doPrepareRender(rdr::Renderer& renderer) override;

private:
    Element::Ptr child;
    float childPadding;

    virtual void onAcquisition() override;
};

} // namespace gui
} // namespace bl

#endif
