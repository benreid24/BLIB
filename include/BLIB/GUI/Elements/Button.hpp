#ifndef BLIB_GUI_ELEMENTS_HPP
#define BLIB_GUI_ELEMENTS_HPP

#include <BLIB/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief A clickable button with a text label
 *
 * @ingroup GUI
 *
 */
class Button : public Element {
public:
    typedef std::shared_ptr<Button> Ptr;

    /**
     * @brief Create a new Button
     *
     * @param text The text to display inside the button
     * @param group The group of the Button
     * @param id The id of this button
     * @return Ptr Pointer to the new Button
     */
    static Ptr create(const std::string& text, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Returns the current text in the button
     *
     */
    const std::string& getText() const;

    /**
     * @brief Set the text in the button
     *
     * @param text The text to render inside the button
     */
    void setText(const std::string& text);

protected:
    /**
     * @brief Create a new Button
     *
     * @param text The text to display inside the button
     * @param group The group of the Button
     * @param id The id of this button
     */
    Button(const std::string& text, const std::string& group, const std::string& id);

    /**
     * @brief Returns the size required to display the full button text
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Marks the button as dirty
     *
     */
    virtual void settingsChanged() override;

    /**
     * @brief Renders the button and text
     *
     * @param target The target to render to
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, Renderer::Ptr renderer) const override;

private:
    std::string text;
};
} // namespace gui
} // namespace bl

#endif