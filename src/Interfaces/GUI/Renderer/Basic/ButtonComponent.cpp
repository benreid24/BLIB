#include <BLIB/Interfaces/GUI/Renderer/Basic/ButtonComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Button.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
ButtonComponent::ButtonComponent()
: Component(HighlightState::HighlightedByMouse) {}

void ButtonComponent::setVisible(bool v) { box.setHidden(!v); }

void ButtonComponent::onElementUpdated() {
    // noop
}

void ButtonComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(settings.fillColor.value_or(sf::Color(85, 85, 85)));
    box.setOutlineColor(settings.outlineColor.value_or(sf::Color::Black));
    box.setOutlineThickness(-settings.outlineThickness.value_or(Button::DefaultOutlineThickness));
}

ecs::Entity ButtonComponent::getEntity() const { return box.entity(); }

void ButtonComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
}

void ButtonComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ButtonComponent::doSceneRemove() { box.removeFromScene(); }

void ButtonComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void ButtonComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
