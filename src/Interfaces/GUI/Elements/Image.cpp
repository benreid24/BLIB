#include <BLIB/Interfaces/GUI/Elements/Image.hpp>

namespace bl
{
namespace gui
{
Image::Ptr Image::create(resource::Resource<sf::Texture>::Ref texture) {
    return Ptr(new Image(texture));
}

Image::Ptr Image::create(const sf::Texture& texture) { return Ptr(new Image(texture)); }

Image::Image(resource::Resource<sf::Texture>::Ref th)
: Element()
, textureHandle(th)
, texture(th.get())
, sprite(*texture)
, fillAcq(false)
, maintainAR(true) {
    getSignal(Action::AcquisitionChanged).willAlwaysCall(std::bind(&Image::setScale, this));
}

Image::Image(const sf::Texture& th)
: Element()
, texture(&th)
, sprite(*texture)
, fillAcq(false)
, maintainAR(true) {
    getSignal(Action::AcquisitionChanged).willAlwaysCall(std::bind(&Image::setScale, this));
}

void Image::scaleToSize(const sf::Vector2f& s) {
    size = s;
    setScale();
    makeDirty();
}

void Image::setFillAcquisition(bool fill, bool mar) {
    fillAcq    = fill;
    maintainAR = mar;
    setScale();
    makeDirty();
}

sf::Vector2i Image::minimumRequisition() const {
    return static_cast<sf::Vector2i>(
        size.value_or(sf::Vector2f(texture->getSize().x, texture->getSize().y)));
}

void Image::doRender(sf::RenderTarget& target, sf::RenderStates states,
                     const Renderer& renderer) const {
    renderer.renderImage(target, states, this, sprite);
}

void Image::setScale() {
    const sf::Vector2f origSize(texture->getSize().x, texture->getSize().y);
    sf::Vector2f area = size.value_or(origSize);
    if (fillAcq) {
        area.x = getAcquisition().width;
        area.y = getAcquisition().height;
    }
    float sx = area.x / origSize.x;
    float sy = area.y / origSize.y;
    if (fillAcq && maintainAR) {
        sx = std::min(sx, sy);
        sy = sx;
    }
    sprite.setScale(sx, sy);
}

} // namespace gui
} // namespace bl