#include <BLIB/Interfaces/Menu/Items/TextItem.hpp>

namespace bl
{
namespace menu
{
TextItem::Ptr TextItem::create(const std::string& text, const sf::Font& font,
                               const sf::Color& color, unsigned int fontSize) {
    return Ptr(new TextItem(text, font, color, fontSize));
}

TextItem::TextItem(const std::string& t, const sf::Font& font, const sf::Color& color,
                   unsigned int fontSize) {
    text.setString(t);
    text.setFont(font);
    text.setFillColor(color);
    text.setCharacterSize(fontSize);
}

sf::Text& TextItem::getTextObject() { return text; }

sf::Vector2f TextItem::getSize() const {
    const sf::FloatRect gb = text.getGlobalBounds();
    return {gb.left + gb.width, gb.top + gb.height};
}

void TextItem::render(sf::RenderTarget& target, sf::RenderStates states,
                      const sf::Vector2f& position) const {
    states.transform.translate(position);
    target.draw(text, states);
}

} // namespace menu
} // namespace bl
