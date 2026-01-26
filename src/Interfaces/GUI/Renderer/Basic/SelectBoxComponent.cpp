#include <BLIB/Interfaces/GUI/Renderer/Basic/SelectBoxComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
SelectBoxComponent::SelectBoxComponent()
: Component(HighlightState::IgnoresMouse) {}

void SelectBoxComponent::setVisible(bool v) { box.setHidden(!v); }

void SelectBoxComponent::onElementUpdated() {
    // noop
}

void SelectBoxComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(settings.fillColor.value_or(sf::Color::Transparent));
    box.setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    box.setOutlineThickness(-settings.outlineThickness.value_or(0.f));
}

ecs::Entity SelectBoxComponent::getEntity() const { return box.entity(); }

void SelectBoxComponent::doCreate(engine::World& world, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(world, {owner.getAcquisition().size.x, owner.getAcquisition().size.y});
    box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);
}

void SelectBoxComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void SelectBoxComponent::doSceneRemove() { box.removeFromScene(); }

void SelectBoxComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().size.x, owner.getAcquisition().size.y});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void SelectBoxComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
