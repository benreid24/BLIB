#include <BLIB/Interfaces/GUI/Renderer/Basic/BasicTooltipProvider.hpp>

#include <BLIB/Cameras/OverlayCamera.hpp>
#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
namespace
{
constexpr unsigned int FontSize = 14;
constexpr float MaxWidth        = 250.f;
constexpr float Padding         = 3.f;
} // namespace

BasicTooltipProvider::BasicTooltipProvider()
: TooltipProvider()
, currentElement(nullptr) {}

void BasicTooltipProvider::update(float) {
    // noop
}

void BasicTooltipProvider::displayTooltip(Element* element, const glm::vec2& mousePos) {
    currentElement = element;

    text.getSection().setCharacterSize(cam::OverlayCamera::getOverlayCoordinateSpace().y * 0.035f);
    text.wordWrap(cam::OverlayCamera::getOverlayCoordinateSpace().x * 0.3f);
    text.getSection().setString(element->getTooltip());
    const sf::FloatRect tb = text.getLocalBounds();
    const glm::vec2 size(tb.left + tb.width + Padding * 2.f, tb.top + tb.height + Padding * 2.f);
    box.setSize(size);

    glm::vec2 pos(mousePos.x, mousePos.y - size.y);
    if (pos.x < 0.f) { pos.x = 0.f; }
    if (pos.y < 0.f) { pos.y = 0.f; }
    if (pos.x + size.x > cam::OverlayCamera::getOverlayCoordinateSpace().x) {
        pos.x = cam::OverlayCamera::getOverlayCoordinateSpace().x - size.x;
    }
    if (pos.y + size.y > cam::OverlayCamera::getOverlayCoordinateSpace().y) {
        pos.y = cam::OverlayCamera::getOverlayCoordinateSpace().y - size.y;
    }

    box.getTransform().setPosition(pos);
    box.setHidden(false);
}

void BasicTooltipProvider::dismissTooltip() { box.setHidden(true); }

void BasicTooltipProvider::doCreate(engine::World& world) {
    box.create(world, {100.f, 100.f});
    box.setFillColor(sf::Color(230, 230, 170));
    box.setOutlineColor(sf::Color::Black);
    box.setOutlineThickness(1.f);
    box.getTransform().setDepth(cam::OverlayCamera::MinDepth);
    box.setHidden(true);

    text.create(world, *Font::get(), "", FontSize);
    text.wordWrap(MaxWidth);
    text.setParent(box);
    text.getTransform().setPosition({Padding, Padding});
}

void BasicTooltipProvider::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
    text.addToScene(overlay, rc::UpdateSpeed::Static);
}

void BasicTooltipProvider::doSceneRemove() { box.removeFromScene(); }

void BasicTooltipProvider::notifyDestroyed(const Element* element) {
    if (element == currentElement) { box.setHidden(true); }
}

} // namespace defcoms
} // namespace gui
} // namespace bl
