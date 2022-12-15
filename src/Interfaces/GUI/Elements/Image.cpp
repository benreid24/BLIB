#include <BLIB/Interfaces/GUI/Elements/Image.hpp>

namespace bl
{
namespace gui
{
Image::Ptr Image::create(resource::Ref<sf::Texture> texture) { return Ptr(new Image(texture)); }

Image::Ptr Image::create(const sf::Texture& texture) { return Ptr(new Image(texture)); }

Image::Image(resource::Ref<sf::Texture> th)
: Element()
, textureHandle(th)
, texture(th.get())
, sprite(*texture)
, fillAcq(false)
, maintainAR(true) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Image::setScale, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&Image::moveCb, this));
}

Image::Image(const sf::Texture& th)
: Element()
, texture(&th)
, sprite(*texture)
, fillAcq(false)
, maintainAR(true) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Image::setScale, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&Image::moveCb, this));
}

void Image::setImage(const sf::Texture& txtr, bool rs) {
    texture = &txtr;
    sprite.setTexture(txtr, true);
    if (rs) {
        size.reset();
        makeDirty();
    }
    setScale();
}

void Image::setImage(const resource::Ref<sf::Texture>& txtr, bool rs) {
    textureHandle = txtr;
    texture       = txtr.get();
    sprite.setTexture(*texture, true);
    if (rs) {
        size.reset();
        makeDirty();
    }
    setScale();
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

sf::Vector2f Image::minimumRequisition() const {
    return size.value_or(sf::Vector2f(texture->getSize().x, texture->getSize().y));
}

void Image::doRender(sf::RenderTarget& target, sf::RenderStates states,
                     const Renderer& renderer) const {
    renderer.renderImage(target, states, this, sprite);
}

void Image::setScale() {
    const sf::Vector2f origSize(texture->getSize().x, texture->getSize().y);
    const sf::Vector2f acqPos(getAcquisition().left, getAcquisition().top);
    const sf::Vector2f acqSize(getAcquisition().width, getAcquisition().height);
    sf::Vector2f imgSize = size.value_or(origSize);
    if (fillAcq) {
        imgSize.x = getAcquisition().width;
        imgSize.y = getAcquisition().height;
    }
    float sx = imgSize.x / origSize.x;
    float sy = imgSize.y / origSize.y;
    if (fillAcq && maintainAR) {
        sx = std::min(sx, sy);
        sy = sx;
    }
    sprite.setScale(sx, sy);
    imgSize.x = sprite.getGlobalBounds().width;
    imgSize.y = sprite.getGlobalBounds().height;
    sprite.setPosition(acqPos + (acqSize - imgSize) * 0.5f);
}

void Image::moveCb() {
    const sf::Vector2f imgSize(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
    const sf::Vector2f acqPos(getAcquisition().left, getAcquisition().top);
    const sf::Vector2f acqSize(getAcquisition().width, getAcquisition().height);
    sprite.setPosition(acqPos + (acqSize - imgSize) * 0.5f);
}

} // namespace gui
} // namespace bl
