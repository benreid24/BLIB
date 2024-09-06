#include <BLIB/Interfaces/GUI/Renderer/Basic/ScrollAreaComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
ScrollAreaComponent::ScrollAreaComponent()
: Component(HighlightState::IgnoresMouse) {}

void ScrollAreaComponent::setVisible(bool v) { box.setHidden(!v); }

void ScrollAreaComponent::onElementUpdated() {
    // noop
}

void ScrollAreaComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(settings.fillColor.value_or(sf::Color::Transparent));
    box.setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    box.setOutlineThickness(-settings.outlineThickness.value_or(0.f));
}

ecs::Entity ScrollAreaComponent::getEntity() const { return box.entity(); }

void ScrollAreaComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
    box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);
}

void ScrollAreaComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ScrollAreaComponent::doSceneRemove() { box.removeFromScene(); }

void ScrollAreaComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void ScrollAreaComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
