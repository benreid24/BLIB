#include <BLIB/Interfaces/Menu/Items/ImageItem.hpp>

namespace bl
{
namespace menu
{
ImageItem::Ptr ImageItem::create(const resource::Ref<sf::Texture>& texture) {
    return Ptr(new ImageItem(texture));
}

ImageItem::ImageItem(const resource::Ref<sf::Texture>& texture)
: texture(texture)
, sprite(*texture) {}

sf::Sprite& ImageItem::getSprite() { return sprite; }

void ImageItem::setTexture(const resource::Ref<sf::Texture>& t) {
    texture = t;
    sprite.setTexture(*texture, true);
}

sf::Vector2f ImageItem::getSize() const {
    return {sprite.getGlobalBounds().width, sprite.getGlobalBounds().height};
}

void ImageItem::render(sf::RenderTarget& target, sf::RenderStates states,
                       const sf::Vector2f& pos) const {
    states.transform.translate(pos);
    target.draw(sprite, states);
}

} // namespace menu
} // namespace bl
