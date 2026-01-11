#include <BLIB/Interfaces/GUI/Renderer/Basic/WindowComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Window.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
WindowComponent::WindowComponent()
: Component(HighlightState::IgnoresMouse) {}

void WindowComponent::setVisible(bool v) { box.setHidden(!v); }

void WindowComponent::onElementUpdated() {
    // noop
}

void WindowComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(settings.fillColor.value_or(sf::Color(75, 75, 75)));
    box.setOutlineColor(settings.outlineColor.value_or(sf::Color(20, 20, 20)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
}

ecs::Entity WindowComponent::getEntity() const { return box.entity(); }

void WindowComponent::doCreate(engine::World& world, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(world, {owner.getAcquisition().size.x, owner.getAcquisition().size.y});
}

void WindowComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void WindowComponent::doSceneRemove() { box.removeFromScene(); }

void WindowComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().size.x, owner.getAcquisition().size.y});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void WindowComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
