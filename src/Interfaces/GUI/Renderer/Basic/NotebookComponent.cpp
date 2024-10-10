#include <BLIB/Interfaces/GUI/Renderer/Basic/NotebookComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Notebook.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
NotebookComponent::NotebookComponent()
: Component(HighlightState::IgnoresMouse) {}

void NotebookComponent::setVisible(bool v) { box.setHidden(!v); }

void NotebookComponent::onElementUpdated() {
    // noop
}

void NotebookComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(settings.fillColor.value_or(sf::Color(120, 120, 120)));
    box.setOutlineColor(settings.outlineColor.value_or(sf::Color::Black));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
}

ecs::Entity NotebookComponent::getEntity() const { return box.entity(); }

void NotebookComponent::doCreate(engine::World& world, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(world, {owner.getAcquisition().width, owner.getAcquisition().height});
    box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);
}

void NotebookComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void NotebookComponent::doSceneRemove() { box.removeFromScene(); }

void NotebookComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void NotebookComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
