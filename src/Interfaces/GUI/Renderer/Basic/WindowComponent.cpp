#include <BLIB/Interfaces/GUI/Renderer/Basic/WindowComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Window.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

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
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color(75, 75, 75))));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color(20, 20, 20))));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
}

ecs::Entity WindowComponent::getEntity() const { return box.entity(); }

void WindowComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
    box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelf);
}

void WindowComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void WindowComponent::doSceneRemove() { box.removeFromScene(); }

void WindowComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void WindowComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
