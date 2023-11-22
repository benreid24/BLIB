#include <BLIB/Interfaces/GUI/Renderer/Basic/SliderComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
SliderComponent::SliderComponent()
: Component(HighlightState::IgnoresMouse) {}

void SliderComponent::setVisible(bool v) { box.setHidden(!v); }

void SliderComponent::onElementUpdated() {
    // noop
}

void SliderComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color(120, 120, 120))));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color(85, 85, 85))));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
}

ecs::Entity SliderComponent::getEntity() const { return box.entity(); }

void SliderComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
}

void SliderComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void SliderComponent::doSceneRemove() { box.removeFromScene(); }

void SliderComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void SliderComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
