#include <BLIB/Interfaces/GUI/Elements/Icon.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
Icon::Ptr Icon::create(Type type, const sf::Vector2f& size, float rotation) {
    return Ptr{new Icon(type, size, rotation)};
}

Icon::Icon(Type type, const sf::Vector2f& size, float rotation)
: type(type)
, ogSize(size)
, size(size)
, rotation(rotation) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Icon::onAcquisition, this));
}

void Icon::setIconSize(const sf::Vector2f& s) {
    ogSize = s;
    size   = s;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

const sf::Vector2f& Icon::getIconSize() const { return size; }

Icon::Type Icon::getType() const { return type; }

void Icon::setRotation(float r) {
    rotation = r;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

float Icon::getRotation() const { return rotation; }

sf::Vector2f Icon::minimumRequisition() const { return ogSize; }

rdr::Component* Icon::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Icon>(*this);
}

void Icon::setFillAcquisition(bool f, bool m) {
    fillAcq    = f;
    maintainAR = m;
    onAcquisition();
}

void Icon::onAcquisition() {
    if (fillAcq) {
        size.x = getAcquisition().width;
        size.y = getAcquisition().height;

        if (maintainAR) {
            const float ar = ogSize.x / ogSize.y;
            if (ar >= 1.f) { size.y /= ar; }
            else { size.x /= ar; }
        }

        if (getComponent()) { getComponent()->onElementUpdated(); }
    }
}

} // namespace gui
} // namespace bl
