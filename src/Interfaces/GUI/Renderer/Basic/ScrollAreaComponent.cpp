#include <BLIB/Interfaces/GUI/Renderer/Basic/ScrollAreaComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

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
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color::Transparent)));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Transparent)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(0.f));
}

ecs::Entity ScrollAreaComponent::getEntity() const { return box.entity(); }

void ScrollAreaComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
    box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);
}

void ScrollAreaComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ScrollAreaComponent::doSceneRemove() { box.removeFromScene(); }

void ScrollAreaComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                            const sf::Vector2f& size) {
    box.setSize({size.x, size.y});
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void ScrollAreaComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
