#include <BLIB/Interfaces/GUI/Renderer/Basic/OverlayHighlightProvider.hpp>

#include <BLIB/Engine/Engine.hpp>
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

            com::Transform2D* pos = enginePtr->ecs().getComponent<com::Transform2D>(
                element->getComponent()->getEntity());
            cover.getTransform().setDepth(pos ? pos->getGlobalDepth() - 0.4f :
                                                cam::OverlayCamera::MinDepth);
            cover.getTransform().setPosition(
                {element->getAcquisition().left, element->getAcquisition().top});
            cover.scaleToSize({element->getAcquisition().width, element->getAcquisition().height});
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
    enginePtr = &engine;
    cover.create(engine, {100.f, 100.f});
    cover.getTransform().setDepth(cam::OverlayCamera::MinDepth);
}

void OverlayHighlightProvider::doSceneAdd(rc::Overlay* scene) {
    cover.addToScene(scene, rc::UpdateSpeed::Dynamic);
    cover.setHidden(true);
}

void OverlayHighlightProvider::doSceneRemove() { cover.removeFromScene(); }

void OverlayHighlightProvider::notifyDestroyed(const Element* destroyed) {
    if (currentElement == destroyed) {
        cover.setHidden(true);
        currentElement = nullptr;
    }
}

} // namespace defcoms
} // namespace gui
} // namespace bl
