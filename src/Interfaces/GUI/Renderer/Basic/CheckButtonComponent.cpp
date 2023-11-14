#include <BLIB/Interfaces/GUI/Renderer/Basic/CheckButtonComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/CheckButton.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
CheckButtonComponent::CheckButtonComponent()
: Component(HighlightState::HighlightedByMouse) {}

void CheckButtonComponent::setVisible(bool v) { box.setHidden(!v); }

void CheckButtonComponent::onElementUpdated() {
    const CheckButton& owner = getOwnerAs<CheckButton>();
    fill.setHidden(!owner.getValue());
}

void CheckButtonComponent::onRenderSettingChange() {
    const CheckButton& owner       = getOwnerAs<CheckButton>();
    const RenderSettings& settings = owner.getRenderSettings();
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color::White)));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Black)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
    box.setSize({owner.getToggleSize(), owner.getToggleSize()});
    fill.setFillColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Black)));
}

ecs::Entity CheckButtonComponent::getEntity() const { return box.entity(); }

void CheckButtonComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*,
                                    Component&) {
    CheckButton& owner = getOwnerAs<CheckButton>();
    box.create(engine, {owner.getToggleSize(), owner.getToggleSize()});
    fill.create(engine, {10.f, 10.f});
    fill.getTransform().setOrigin({5.f, 5.f});
    fill.getOverlayScaler().positionInParentSpace({0.5f, 0.5f});
    fill.getOverlayScaler().scaleToSizePercent({0.6f, 0.6f});
    fill.setParent(box);
}

void CheckButtonComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
    fill.addToScene(overlay, rc::UpdateSpeed::Static);
}

void CheckButtonComponent::doSceneRemove() { box.removeFromScene(); }

void CheckButtonComponent::handleAcquisition(const sf::Vector2f& posFromParent,
                                             const sf::Vector2f& posFromWindow,
                                             const sf::Vector2f&) {
    handleMove(posFromParent, posFromWindow);
}

void CheckButtonComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    const CheckButton& owner = getOwnerAs<CheckButton>();
    box.getTransform().setPosition(
        {posFromParent.x,
         posFromParent.y + owner.getAcquisition().height * 0.5f - owner.getToggleSize() * 0.5f});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
