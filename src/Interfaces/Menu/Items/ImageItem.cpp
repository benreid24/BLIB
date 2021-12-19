#include <BLIB/Interfaces/Menu/Items/ImageItem.hpp>

namespace bl
{
namespace menu
{
ImageItem::Ptr ImageItem::create(const resource::Resource<sf::Texture>::Ref& texture) {
    return Ptr(new ImageItem(texture));
}

ImageItem::ImageItem(const resource::Resource<sf::Texture>::Ref& texture)
: texture(texture)
, sprite(*texture) {}

sf::Sprite& ImageItem::getSprite() { return sprite; }

sf::Vector2f ImageItem::getSize() const {
    return {sprite.getGlobalBounds().width, sprite.getGlobalBounds().height};
}

void ImageItem::render(sf::RenderTarget& target, sf::RenderStates states,
                       const sf::Vector2f& position) const {
    states.transform.translate(position);
    target.draw(sprite, states);
}

} // namespace menu
} // namespace bl
