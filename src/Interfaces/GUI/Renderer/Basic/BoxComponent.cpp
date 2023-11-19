#include <BLIB/Interfaces/GUI/Renderer/Basic/BoxComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

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
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color::Transparent)));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Transparent)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(0.f));
}

ecs::Entity BoxComponent::getEntity() const { return box.entity(); }

void BoxComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
}

void BoxComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void BoxComponent::doSceneRemove() { box.removeFromScene(); }

void BoxComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                     const sf::Vector2f& size) {
    box.setSize({size.x, size.y});
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void BoxComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
