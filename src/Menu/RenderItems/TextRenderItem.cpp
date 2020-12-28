#include <BENG/Menu/RenderItems/TextRenderItem.hpp>

namespace bg
{
namespace menu
{
TextRenderItem::Ptr TextRenderItem::create(const sf::Text& text) {
    return Ptr(new TextRenderItem(text));
}

TextRenderItem::TextRenderItem(const sf::Text& text)
: text(text) {}

sf::Text& TextRenderItem::getTextObject() { return text; }

sf::Vector2f TextRenderItem::getSize() const {
    return {text.getGlobalBounds().width, text.getGlobalBounds().height};
}

void TextRenderItem::render(sf::RenderTarget& target, sf::RenderStates states,
                            const sf::Vector2f& position) const {
    text.setPosition(position);
    target.draw(text, states);
}

} // namespace menu
} // namespace bg
