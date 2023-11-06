#include <BLIB/Interfaces/GUI/Renderer/Basic/OverlayHighlightProvider.hpp>

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
OverlayHighlightProvider::OverlayHighlightProvider()
: currentElement(nullptr) {}

void OverlayHighlightProvider::update(float) {}

void OverlayHighlightProvider::notifyUIState(Element* element, rdr::Component::UIState state) {
    if (element->getComponent()) {
        if (element != currentElement || state != currentState) {
            currentElement = element;
            currentState   = state;

            cover.setParent(element->getComponent()->getEntity());
            cover.setHidden(false);

            switch (state) {
            case rdr::Component::UIState::Highlighted:
                cover.setFillColor(sfcol(sf::Color(255, 255, 255, 100)));
                break;
            case rdr::Component::UIState::Pressed:
                cover.setFillColor(sfcol(sf::Color(30, 30, 30, 100)));
                break;
            default:
                cover.setHidden(true);
                break;
            }
        }
    }
}

void OverlayHighlightProvider::doCreate(engine::Engine& engine) {
    cover.create(engine, {100.f, 100.f});
}

void OverlayHighlightProvider::doSceneAdd(rc::Overlay* scene) {
    cover.addToScene(scene, rc::UpdateSpeed::Static);
    cover.getOverlayScaler().scaleToSizePercent({1.f, 1.f});
    cover.setHidden(true);
}

void OverlayHighlightProvider::doSceneRemove() { cover.removeFromScene(); }

} // namespace defcoms
} // namespace gui
} // namespace bl
