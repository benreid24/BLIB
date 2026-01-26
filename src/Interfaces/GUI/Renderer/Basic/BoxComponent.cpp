#include <BLIB/Interfaces/GUI/Renderer/Basic/BoxComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
BoxComponent::BoxComponent()
: Component(HighlightState::IgnoresMouse) {}

void BoxComponent::setVisible(bool v) { box.setHidden(!v); }

void BoxComponent::onElementUpdated() {
    Box& owner = getOwnerAs<Box>();
    box.getOverlayScaler().setScissorMode(owner.isViewConstrained() ?
                                              com::OverlayScaler::ScissorSelfConstrained :
                                              com::OverlayScaler::ScissorInherit);
}

void BoxComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(settings.fillColor.value_or(sf::Color::Transparent));
    box.setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    box.setOutlineThickness(-settings.outlineThickness.value_or(0.f));
}

ecs::Entity BoxComponent::getEntity() const { return box.entity(); }

void BoxComponent::doCreate(engine::World& world, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(world, {owner.getAcquisition().size.x, owner.getAcquisition().size.y});
}

void BoxComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void BoxComponent::doSceneRemove() { box.removeFromScene(); }

void BoxComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().size.x, owner.getAcquisition().size.y});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void BoxComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
