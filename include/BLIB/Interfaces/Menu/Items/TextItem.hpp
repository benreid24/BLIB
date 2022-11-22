#ifndef BLIB_MENU_RENDERITEMS_TEXTITEM_HPP
#define BLIB_MENU_RENDERITEMS_TEXTITEM_HPP

#include <BLIB/Interfaces/Menu/Item.hpp>
#include <SFML/Graphics/Text.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief RenderItem to display text
 *
 * @ingroup Menu
 *
 */
class TextItem : public Item {
public:
    typedef std::shared_ptr<TextItem> Ptr;

    /**
     * @brief Create a new TextItem from the sf::Text object
     *
     * @param text The string to display
     * @param font The font to use
     * @param color The color of the text
     * @param fontSize The font size
     * @return Ptr The created text menu item
     */
    static Ptr create(const std::string& text, const sf::Font& font,
                      const sf::Color& color = sf::Color::Black, unsigned int fontSize = 30);

    /**
     * @brief Destroy the Text Item object
     *
     */
    virtual ~TextItem() = default;

    /**
     * @brief Returns a modifiable reference to the Text object
     *
     */
    sf::Text& getTextObject();

    /**
     * @see Item::getSize
     *
     */
    virtual sf::Vector2f getSize() const override;

protected:
    /**
     * @brief Create a new TextItem from the sf::Text object
     *
     * @param text The string to display
     * @param font The font to use
     * @param color The color of the text
     * @param fontSize The font size
     */
    TextItem(const std::string& text, const sf::Font& font, const sf::Color& color,
             unsigned int fontSize);

    /**
     * @see Item::render
     *
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const override;

private:
    sf::Text text;
};

} // namespace menu
} // namespace bl

#endif
