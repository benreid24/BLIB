#include <BLIB/Menu/RenderItems/SpriteRenderItem.hpp>

namespace bl
{
namespace menu
{
SpriteRenderItem::Ptr SpriteRenderItem::create(const sf::Sprite& s) {
    return Ptr(new SpriteRenderItem(s));
}

SpriteRenderItem::SpriteRenderItem(const sf::Sprite& s)
: sprite(s) {}

sf::Sprite& SpriteRenderItem::getSprite() { return sprite; }

sf::Vector2f SpriteRenderItem::getSize() const {
    return {sprite.getGlobalBounds().width, sprite.getGlobalBounds().height};
}

void SpriteRenderItem::render(sf::RenderTarget& target, sf::RenderStates states,
                              const sf::Vector2f& position) const {
    sprite.setPosition(position);
    target.draw(sprite, states);
}

} // namespace menu
} // namespace bl