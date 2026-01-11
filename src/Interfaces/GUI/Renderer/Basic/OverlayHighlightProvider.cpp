#include <BLIB/Interfaces/GUI/Renderer/Basic/OverlayHighlightProvider.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Render/Config/PipelineIds.hpp>

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

            switch (state) {
            case rdr::Component::UIState::Highlighted:
                cover.setFillColor(sf::Color(255, 255, 255, 100));
                break;
            case rdr::Component::UIState::Pressed:
                cover.setFillColor(sf::Color(30, 30, 30, 100));
                break;
            default:
                cover.setHidden(true);
                return;
            }

            ecs::Entity parent    = element->getComponent()->getEntity();
            com::Transform2D* pos = enginePtr->ecs().getComponent<com::Transform2D>(parent);
            if (pos) {
                cover.getTransform().setDepth(
                    std::max(-10.f, cam::OverlayCamera::MinDepth - pos->getGlobalDepth()));
            }
            else { cover.getTransform().setDepth(cam::OverlayCamera::MinDepth); }
            cover.scaleToSize({element->getAcquisition().width, element->getAcquisition().height});
            cover.setParent(parent);
            cover.setHidden(false);
        }
    }
}

void OverlayHighlightProvider::doCreate(engine::World& world) {
    enginePtr = &world.engine();
    cover.create(world, {100.f, 100.f});
    world.engine().ecs().setEntityParentDestructionBehavior(
        cover.entity(), ecs::ParentDestructionBehavior::OrphanedByParent);
    cover.getTransform().setDepth(cam::OverlayCamera::MinDepth);
    cover.material().setPipeline(
        rc::mat::MaterialPipelineSettings(rc::cfg::PipelineIds::Unlit2DGeometryNoDepthWrite)
            .build());
}

void OverlayHighlightProvider::doSceneAdd(rc::Overlay* scene) {
    cover.addToScene(scene, rc::UpdateSpeed::Dynamic);
    cover.setHidden(true);
}

void OverlayHighlightProvider::doSceneRemove() { cover.removeFromScene(); }

void OverlayHighlightProvider::notifyDestroyed(const Element* destroyed) {
    if (currentElement == destroyed ||
        (currentElement && currentElement->isInParentTree(destroyed))) {
        cover.removeParent();
        cover.setHidden(true);
        currentElement = nullptr;
    }
}

} // namespace defcoms
} // namespace gui
} // namespace bl
