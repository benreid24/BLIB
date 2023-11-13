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

void WindowComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
}

void WindowComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void WindowComponent::doSceneRemove() { box.removeFromScene(); }

void WindowComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                        const sf::Vector2f& size) {
    box.setSize({size.x, size.y});
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void WindowComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
