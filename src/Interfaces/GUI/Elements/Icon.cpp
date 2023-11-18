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
, size(size)
, rotation(rotation) {}

void Icon::setIconSize(const sf::Vector2f& s) {
    size = s;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

const sf::Vector2f& Icon::getIconSize() const { return size; }

Icon::Type Icon::getType() const { return type; }

void Icon::setRotation(float r) {
    rotation = r;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

float Icon::getRotation() const { return rotation; }

sf::Vector2f Icon::minimumRequisition() const { return size; }

rdr::Component* Icon::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Icon>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

} // namespace gui
} // namespace bl
