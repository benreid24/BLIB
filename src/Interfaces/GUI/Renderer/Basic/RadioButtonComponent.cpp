#include <BLIB/Interfaces/GUI/Renderer/Basic/RadioButtonComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/RadioButton.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
RadioButtonComponent::RadioButtonComponent()
: Component(HighlightState::HighlightedByMouse) {}

void RadioButtonComponent::setVisible(bool v) { circle.setHidden(!v); }

void RadioButtonComponent::onElementUpdated() {
    const RadioButton& owner = getOwnerAs<RadioButton>();
    fill.setHidden(!owner.getValue());
}

void RadioButtonComponent::onRenderSettingChange() {
    const RadioButton& owner       = getOwnerAs<RadioButton>();
    const RenderSettings& settings = owner.getRenderSettings();
    circle.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color::White)));
    circle.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Black)));
    circle.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
    circle.setRadius(owner.getToggleSize() * 0.5f);
    fill.setFillColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Black)));
}

ecs::Entity RadioButtonComponent::getEntity() const { return dummy.entity(); }

void RadioButtonComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*,
                                    Component&) {
    RadioButton& owner = getOwnerAs<RadioButton>();
    dummy.create(engine);
    circle.create(engine, owner.getToggleSize() * 0.5f);
    circle.setParent(dummy);
    fill.create(engine, 5.f);
    fill.getTransform().setOrigin({5.f, 5.f});
    fill.getOverlayScaler().positionInParentSpace({0.5f, 0.5f});
    fill.getOverlayScaler().scaleToSizePercent({0.55f, 0.55f});
    fill.setParent(circle);
}

void RadioButtonComponent::doSceneAdd(rc::Overlay* overlay) {
    circle.addToScene(overlay, rc::UpdateSpeed::Static);
    fill.addToScene(overlay, rc::UpdateSpeed::Static);
}

void RadioButtonComponent::doSceneRemove() { circle.removeFromScene(); }

void RadioButtonComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                             const sf::Vector2f& size) {
    const RadioButton& owner = getOwnerAs<RadioButton>();
    dummy.setSize({size.x, size.y});
    circle.getTransform().setPosition({0.f, size.y * 0.5f - owner.getToggleSize() * 0.5f});
    dummy.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void RadioButtonComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    dummy.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
