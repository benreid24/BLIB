#include <BLIB/Interfaces/GUI/Renderer/Basic/AnimationComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Animation.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
AnimationComponent::AnimationComponent()
: Component(HighlightState::IgnoresMouse) {}

void AnimationComponent::setVisible(bool v) { anim.setHidden(!v); }

void AnimationComponent::onElementUpdated() {
    Animation& owner = getOwnerAs<Animation>();
    if (owner.getAnimation().get() != source.get()) {
        BL_LOG_WARN << "Changing animations is not yet supported";
        // TODO - add support to change animation
    }

    if (owner.getSize().has_value()) {
        const sf::Vector2f& size = owner.getSize().value();
        anim.getTransform().setScale(
            {size.x / anim.getLocalSize().x, size.y / anim.getLocalSize().y});
    }
    else { anim.getTransform().setScale(1.f); }
}

void AnimationComponent::onRenderSettingChange() { setPosition(); }

ecs::Entity AnimationComponent::getEntity() const { return anim.entity(); }

void AnimationComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Animation& owner = getOwnerAs<Animation>();
    source           = owner.getAnimation();
    anim.createWithUniquePlayer(engine, owner.getAnimation(), true, true);
}

void AnimationComponent::doSceneAdd(rc::Overlay* overlay) {
    anim.addToScene(overlay, rc::UpdateSpeed::Static);
}

void AnimationComponent::doSceneRemove() { anim.removeFromScene(); }

void AnimationComponent::handleAcquisition(const sf::Vector2f&, const sf::Vector2f&,
                                           const sf::Vector2f&) {
    setPosition();
}

void AnimationComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    setPosition();
}

void AnimationComponent::setPosition() {
    Animation& owner               = getOwnerAs<Animation>();
    const RenderSettings& settings = owner.getRenderSettings();
    const sf::Vector2f pos         = RenderSettings::calculatePosition(
                                 settings.horizontalAlignment.value_or(RenderSettings::Center),
                                 settings.verticalAlignment.value_or(RenderSettings::Center),
                                 owner.getAcquisition(),
                                 {anim.getLocalSize().x, anim.getLocalSize().y}) +
                             owner.getLocalPosition();
    anim.getTransform().setPosition({pos.x, pos.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
