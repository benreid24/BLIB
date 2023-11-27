#include <BLIB/Interfaces/GUI/Renderer/Basic/SeparatorComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Separator.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
SeparatorComponent::SeparatorComponent()
: Component(HighlightState::IgnoresMouse) {}

void SeparatorComponent::setVisible(bool v) { sep.setHidden(!v); }

void SeparatorComponent::onElementUpdated() {
    // noop
}

void SeparatorComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Separator>().getRenderSettings();
    sep.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color::Black)));
}

ecs::Entity SeparatorComponent::getEntity() const { return sep.entity(); }

void SeparatorComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    sep.create(engine, computeSize());
}

void SeparatorComponent::doSceneAdd(rc::Overlay* overlay) {
    sep.addToScene(overlay, rc::UpdateSpeed::Static);
}

void SeparatorComponent::doSceneRemove() { sep.removeFromScene(); }

void SeparatorComponent::handleAcquisition() {
    sep.setSize(computeSize());
    sep.getTransform().setPosition(computePos());
}

void SeparatorComponent::handleMove() { sep.getTransform().setPosition(computePos()); }

glm::vec2 SeparatorComponent::computeSize() {
    Separator& owner = getOwnerAs<Separator>();
    return {owner.getDirection() == Separator::Horizontal ? owner.getAcquisition().width :
                                                            owner.getThickness(),
            owner.getDirection() == Separator::Vertical ? owner.getAcquisition().height :
                                                          owner.getThickness()};
}

glm::vec2 SeparatorComponent::computePos() {
    Separator& owner               = getOwnerAs<Separator>();
    const RenderSettings& settings = owner.getRenderSettings();
    const glm::vec2 size           = computeSize();
    const sf::Vector2f pos         = owner.getLocalPosition() +
                             RenderSettings::calculatePosition(
                                 settings.horizontalAlignment.value_or(RenderSettings::Center),
                                 settings.verticalAlignment.value_or(RenderSettings::Center),
                                 owner.getAcquisition(),
                                 {size.x, size.y});
    return {pos.x, pos.y};
}

} // namespace defcoms
} // namespace gui
} // namespace bl
