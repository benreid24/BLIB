#ifndef BLIB_GUI_ELEMENTS_HPP
#define BLIB_GUI_ELEMENTS_HPP

#include <BENG/GUI/Elements/Container.hpp>

namespace bg
{
namespace gui
{
/**
 * @brief A clickable button with a single child element, typically a Label
 *
 * @ingroup GUI
 *
 */
class Button : public Container {
public:
    typedef std::shared_ptr<Button> Ptr;

    static constexpr int DefaultOutlineThickness = 2;

    /**
     * @brief Create a new Button with a label inside of it
     *
     * @param text The text to display inside the button
     * @param group The group of the Button
     * @param id The id of this button
     * @return Ptr Pointer to the new Button
     */
    static Ptr create(const std::string& text, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Create a new Button with any element inside. Typically used for images
     *
     * @param child The child element to put inside the button
     * @param group The group of the Button
     * @param id The id of this button
     * @return Ptr Pointer to the new Button
     */
    static Ptr create(Element::Ptr child, const std::string& group = "",
                      const std::string& id = "");

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
     * @param group The group of the Button
     * @param id The id of this button
     */
    Button(Element::Ptr c, const std::string& group, const std::string& id);

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
     * @brief Sets the acquisition of the child element to its own acquisition
     *
     */
    virtual void onAcquisition() override;

    /**
     * @brief Renders the button and text/child
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

    /**
     * @brief Calls add() on the child method. Should be called in create() for any derived
     *        classes. add() cannot be called until the object is fully constructed
     *
     */
    void addChild();

private:
    Element::Ptr child;
};
} // namespace gui
} // namespace bg

#endif