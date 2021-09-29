#ifndef BLIB_GUI_ELEMENTS_HPP
#define BLIB_GUI_ELEMENTS_HPP

#include <BLIB/Interfaces/GUI/Elements/Container.hpp>
#include <BLIB/Interfaces/GUI/Event.hpp>

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
class Button : public Element {
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
    Element::Ptr getChild() const;

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
     * @brief Renders the button and text/child
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    Element::Ptr child;

    void onAcquisition();
};

} // namespace gui
} // namespace bl

#endif
