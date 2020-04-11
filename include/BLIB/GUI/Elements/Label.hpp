#ifndef BLIB_GUI_ELEMENTS_LABEL_HPP
#define BLIB_GUI_ELEMENTS_LABEL_HPP

#include <BLIB/GUI/Elements/Element.hpp>
#include <BLIB/Resources/ResourceLoader.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Display only text element. Used to display text in a GUI
 *
 * @ingroup GUI
 *
 */
class Label : public Element {
public:
    /**
     * @brief Construct a new Label
     *
     * @param text The text to display
     * @param group The group the element is in
     * @param id The id of this element
     */
    Label(const std::string& text, const std::string& group = "", const std::string& id = "");

    /**
     * @brief Destroy the Label object
     *
     */
    virtual ~Label() = default;

    /**
     * @brief Set the text displayed in the label
     *
     */
    void setText(const std::string& text);

    /**
     * @brief Set the font used by the label. Note that a default font is used if this isn't
     *        called. The default font may be left out of the build by defining BLIB_NO_FONT
     *
     * @param font Resource managed font to use
     */
    void setFont(bl::Resource<sf::Font>::Ref font);

    /**
     * @brief Set the character size. Default is 12
     *
     * @param size Point size
     */
    void setCharacterSize(unsigned int size);

    /**
     * @brief Set the text fill and outline color
     *
     * @param fill Fill color of the text
     * @param outline Outline color of the text
     *
     */
    void setColor(sf::Color fill, sf::Color outline);

    /**
     * @brief Set the outline thickness of the text, in pixels
     *
     * @param thickness Thickness in pixels
     */
    void setOutlineThickness(unsigned int thickness);

    /**
     * @brief Set the style of the text. See sf::Text::Style
     *
     * @param style Style to set
     */
    void setStyle(sf::Uint32 style);

    /**
     * @brief Render the Label to the given target
     *
     * @param target The target to render to
     * @param renderer The renderer to use
     */
    virtual void render(sf::RenderTarget& target, Renderer::Ptr renderer) const override;

    /**
     * @brief Exposes the underlying text object for a Renderer
     *
     */
    const sf::Text& getRenderText() const;

private:
    std::string text;
    bl::Resource<sf::Font>::Ref font;
    sf::Text renderText;

    /**
     * @brief Puts the text into the renderText and marks dirty if the text exceeds the
     *        assigned acquisition
     *
     */
    void refresh();
};

} // namespace gui
} // namespace bl

#endif