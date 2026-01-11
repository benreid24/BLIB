#include <BLIB/Interfaces/GUI/Elements/Image.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
Image::Ptr Image::create(resource::Ref<sf::Image> texture) { return Ptr(new Image(texture)); }

Image::Ptr Image::create(const sf::Image& texture) { return Ptr(new Image(texture)); }

Image::Image(resource::Ref<sf::Image> th)
: Element()
, textureHandle(th)
, texture(th.get())
, offset(0.f, 0.f)
, scale(1.f, 1.f)
, fillAcq(false)
, maintainAR(true) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Image::setScale, this));
}

Image::Image(const sf::Image& th)
: Element()
, texture(&th)
, offset(0.f, 0.f)
, scale(1.f, 1.f)
, fillAcq(false)
, maintainAR(true) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Image::setScale, this));
}

void Image::setImage(const sf::Image& txtr, bool rs) {
    texture = &txtr;
    if (rs) {
        size.reset();
        makeDirty();
    }
    setScale();
}

void Image::setImage(const resource::Ref<sf::Image>& txtr, bool rs) {
    textureHandle = txtr;
    texture       = txtr.get();
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

rdr::Component* Image::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Image>(*this);
}

void Image::setScale() {
    const sf::Vector2f origSize(texture->getSize().x, texture->getSize().y);
    const sf::Vector2f acqPos(getAcquisition().position.x, getAcquisition().position.y);
    const sf::Vector2f acqSize(getAcquisition().size.x, getAcquisition().size.y);
    sf::Vector2f imgSize = size.value_or(origSize);
    if (fillAcq) {
        imgSize.x = getAcquisition().size.x;
        imgSize.y = getAcquisition().size.y;
    }
    scale.x = imgSize.x / origSize.x;
    scale.y = imgSize.y / origSize.y;
    if (fillAcq && maintainAR) {
        scale.x = std::min(scale.x, scale.y);
        scale.y = scale.x;
    }
    imgSize.x = static_cast<float>(texture->getSize().x) * scale.x;
    imgSize.y = static_cast<float>(texture->getSize().y) * scale.y;
    offset    = (acqSize - imgSize) * 0.5f;

    if (getComponent()) { getComponent()->onElementUpdated(); }
}

} // namespace gui
} // namespace bl
