#include <BLIB/Interfaces/GUI/Renderer/Basic/OverlayFlashProvider.hpp>

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Math.hpp>
#include <BLIB/Render/Primitives/Color.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
namespace defcoms
{
OverlayFlashProvider::OverlayFlashProvider()
: enginePtr(nullptr)
, currentElement(nullptr)
, flashTime(0.f) {}

void OverlayFlashProvider::update(float dt) {
    if (currentElement != nullptr) {
        flashTime += dt;
        if (flashTime > 0.5f) {
            flashTime      = 0.f;
            currentElement = nullptr;
            cover.setHidden(true);
        }
        else {
            cover.setFillColor(sfcol(sf::Color(
                255, 255, 255, 50.f + std::abs(std::sin(flashTime * 4.f * math::Pi) * 120.f))));
        }
    }
}

void OverlayFlashProvider::flashElement(Element* element) {
    if (element != currentElement) {
        currentElement = element;
        flashTime      = 0.f;
        cover.setHidden(false);

        com::Transform2D* pos =
            enginePtr->ecs().getComponent<com::Transform2D>(element->getComponent()->getEntity());
        cover.getTransform().setDepth(
            std::min(pos ? pos->getGlobalDepth() - 600.f : cam::OverlayCamera::MinDepth,
                     cam::OverlayCamera::MinDepth));
        cover.getTransform().setPosition(
            {element->getAcquisition().left, element->getAcquisition().top});
        cover.scaleToSize({element->getAcquisition().width, element->getAcquisition().height});
    }
}

void OverlayFlashProvider::doCreate(engine::Engine& engine) {
    enginePtr = &engine;
    cover.create(engine, {100.f, 100.f});
}

void OverlayFlashProvider::doSceneAdd(rc::Overlay* scene) {
    cover.addToScene(scene, rc::UpdateSpeed::Static);
}

void OverlayFlashProvider::doSceneRemove() { cover.removeFromScene(); }

void OverlayFlashProvider::notifyDestroyed(const Element* element) {
    if (element == currentElement) {
        currentElement = nullptr;
        flashTime      = 0.f;
        cover.setHidden(true);
    }
}

} // namespace defcoms
} // namespace gui
} // namespace bl
