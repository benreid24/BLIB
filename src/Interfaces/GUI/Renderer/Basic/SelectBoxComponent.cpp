#include <BLIB/Interfaces/GUI/Renderer/Basic/SelectBoxComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

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
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color::Transparent)));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Transparent)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(0.f));
}

ecs::Entity SelectBoxComponent::getEntity() const { return box.entity(); }

void SelectBoxComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
    box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);
}

void SelectBoxComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void SelectBoxComponent::doSceneRemove() { box.removeFromScene(); }

void SelectBoxComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                           const sf::Vector2f& size) {
    box.setSize({size.x, size.y});
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void SelectBoxComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
