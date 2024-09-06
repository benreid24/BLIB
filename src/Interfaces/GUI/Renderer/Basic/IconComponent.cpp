#include <BLIB/Interfaces/GUI/Renderer/Basic/IconComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Icon.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
IconComponent::IconComponent()
: Component(HighlightState::IgnoresMouse) {}

void IconComponent::setVisible(bool v) { icon.value().setHidden(!v); }

void IconComponent::onElementUpdated() {
    Icon& owner = getOwnerAs<Icon>();
    icon.value().setSize({owner.getIconSize().x, owner.getIconSize().y});
    icon.value().getTransform().setOrigin(icon.value().getSize() * 0.5f);
    icon.value().getTransform().setRotation(owner.getRotation());
    setPosition();
}

void IconComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    icon.value().setFillColor(settings.fillColor.value_or(sf::Color::Black));
    icon.value().setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    icon.value().setOutlineThickness(-settings.outlineThickness.value_or(0.f));
    setPosition();
}

ecs::Entity IconComponent::getEntity() const { return icon.value().entity(); }

void IconComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    Icon& owner = getOwnerAs<Icon>();
    icon.emplace(owner.getType(), glm::vec2{owner.getIconSize().x, owner.getIconSize().y});
    icon.value().create(engine);
}

void IconComponent::doSceneAdd(rc::Overlay* overlay) {
    icon.value().addToScene(overlay, rc::UpdateSpeed::Static);
}

void IconComponent::doSceneRemove() { icon.value().removeFromScene(); }

void IconComponent::handleAcquisition() { setPosition(); }

void IconComponent::handleMove() { setPosition(); }

void IconComponent::setPosition() {
    Icon& owner                    = getOwnerAs<Icon>();
    const RenderSettings& settings = owner.getRenderSettings();

    const sf::Vector2f pos = RenderSettings::calculatePosition(
                                 settings.horizontalAlignment.value_or(RenderSettings::Center),
                                 settings.verticalAlignment.value_or(RenderSettings::Center),
                                 owner.getAcquisition(),
                                 owner.getIconSize()) +
                             owner.getLocalPosition() + owner.getIconSize() * 0.5f;

    icon.value().getTransform().setPosition({pos.x, pos.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
