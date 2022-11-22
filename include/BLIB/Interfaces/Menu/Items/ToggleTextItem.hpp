#include <BLIB/Interfaces/Menu/Items/TextItem.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Text item with a boolean toggle state. Renders a checkbox to the side of the text
 *
 * @ingroup Menu
 *
 */
class ToggleTextItem : public TextItem {
public:
    using Ptr = std::shared_ptr<ToggleTextItem>;

    /**
     * @brief Creates a ToggleTextItem using parameters for TextItem
     *
     * @param text The string to render
     * @param font The font to use
     * @param color The color to make the text
     * @param fontSize The size of the text
     * @return Ptr The new menu item
     */
    static Ptr create(const std::string& text, const sf::Font& font,
                      const sf::Color& color = sf::Color::Black, unsigned int fontSize = 30);

    /**
     * @brief Destroy the Toggle Text Item object
     *
     */
    virtual ~ToggleTextItem() = default;

    /**
     * @brief Returns whether or not the toggle is currently in the checked state
     *
     * @return True if the box is checked, false otherwise
     */
    bool isChecked() const;

    /**
     * @brief Sets whether or not the box is currently checked. This is called for you
     *
     * @param checked True to check the box, false to uncheck
     */
    void setChecked(bool checked);

    /**
     * @brief Sets the settings to render the checkbox with. Note that the outline is rendered
     *        inside of the width and takes up no extra space
     *
     * @param fillColor Color of the inside of the box. Defaults to the negative of the text color
     * @param borderColor Color of the box border. Defaults to the text color
     * @param width Width and height of the square. Defaults to fontSize
     * @param borderThickness Thickness of the box outline. Defaults to 2.f
     * @param padding Space between text and box. Defaults to fontSize / 2.f
     * @param showOnLeft True for box on left of the text, false for right. Default is right side
     */
    void setBoxProperties(sf::Color fillColor, sf::Color borderColor, float width,
                          float borderThickness, float padding, bool showOnLeft);

    /**
     * @brief Returns the size of the text plus the box and padding
     *
     * @return sf::Vector2f The size the menu item takes up
     */
    virtual sf::Vector2f getSize() const override;

protected:
    /**
     * @brief Creates a ToggleTextItem using parameters for TextItem
     *
     * @param text The string to render
     * @param font The font to use
     * @param color The color to make the text
     * @param fontSize The size of the text
     */
    ToggleTextItem(const std::string& text, const sf::Font& font, const sf::Color& color,
                   unsigned int fontSize);

    /**
     * @brief Renders the text and checkbox
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param position The position to render at
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const override;

private:
    bool checked;
    bool leftSide;
    float padding;
    sf::RectangleShape box;
    sf::RectangleShape innerBox;
    float textOffset;
};

} // namespace menu
} // namespace bl
