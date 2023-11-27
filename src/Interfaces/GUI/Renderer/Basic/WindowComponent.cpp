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
: ShapeBatchProvider(HighlightState::IgnoresMouse) {}

void WindowComponent::setVisible(bool v) {
    batch.setHidden(!v);
    dummy.setHidden(!v);
}

void WindowComponent::onElementUpdated() {
    // noop
}

void WindowComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    batchRect.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color(75, 75, 75))));
    batchRect.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color(20, 20, 20))));
    batchRect.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
}

ecs::Entity WindowComponent::getEntity() const { return dummy.entity(); }

void WindowComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    setEnabled(true);
    dummy.create(engine);
    batch.create(engine, 128);
    batch.setParent(dummy);
    batchRect.create(engine, batch, {owner.getAcquisition().width, owner.getAcquisition().height});
    dummy.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelf);
}

void WindowComponent::doSceneAdd(rc::Overlay* overlay) {
    batch.addToScene(overlay, rc::UpdateSpeed::Static);
}

void WindowComponent::doSceneRemove() { batch.removeFromScene(); }

void WindowComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    batchRect.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    dummy.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void WindowComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    dummy.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
