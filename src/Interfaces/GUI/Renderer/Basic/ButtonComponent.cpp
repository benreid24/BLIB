#include <BLIB/Interfaces/GUI/Renderer/Basic/ButtonComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Button.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

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
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color(85, 85, 85))));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Black)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(Button::DefaultOutlineThickness));
}

ecs::Entity ButtonComponent::getEntity() const { return box.entity(); }

void ButtonComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
}

void ButtonComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ButtonComponent::doSceneRemove() { box.removeFromScene(); }

void ButtonComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                        const sf::Vector2f& size) {
    box.setSize({size.x, size.y});
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void ButtonComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl