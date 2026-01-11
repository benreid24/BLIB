#include <BLIB/Interfaces/GUI/Renderer/Basic/LabelComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/GUI.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
LabelComponent::LabelComponent()
: Component(HighlightState::IgnoresMouse) {}

void LabelComponent::setVisible(bool v) { text.setHidden(!v); }

void LabelComponent::onElementUpdated() {
    auto& owner = getOwnerAs<Label>();
    text.getSection().setString(owner.getText());

    switch (owner.getTextWrapBehavior()) {
    case Label::WrapFixedWidth:
        text.wordWrap(owner.getTextWrapWidth());
        break;
    case Label::WrapToAcquisition:
        text.wordWrap(100.f);
        break;
    case Label::WrapDisabled:
    default:
        text.stopWordWrap();
        break;
    }

    reposition();
}

void LabelComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Label>().renderSettings();
    auto& sec                      = text.getSection();
    text.setFont(*settings.font.value_or(Font::get()));
    sec.setCharacterSize(settings.characterSize.value_or(Label::DefaultFontSize));
    sec.setFillColor(settings.fillColor.value_or(sf::Color::Black));
    sec.setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    sec.setOutlineThickness(settings.outlineThickness.value_or(0));
    sec.setStyle(settings.style.value_or(sf::Text::Regular));
    reposition();
}

ecs::Entity LabelComponent::getEntity() const { return text.entity(); }

void LabelComponent::doCreate(engine::World& world, rdr::Renderer&) {
    const RenderSettings& settings = getOwnerAs<Label>().renderSettings();
    text.create(world, *settings.font.value_or(Font::get()));
}

void LabelComponent::doSceneAdd(rc::Overlay* overlay) {
    text.addToScene(overlay, rc::UpdateSpeed::Static);
}

void LabelComponent::doSceneRemove() { text.removeFromScene(); }

void LabelComponent::handleAcquisition() {
    auto& owner = getOwnerAs<Label>();
    if (owner.getTextWrapBehavior() == Label::WrapToAcquisition) {
        if (text.getWordWrapWidth() != owner.getAcquisition().size.x) {
            text.wordWrap(std::max(owner.getAcquisition().size.x, 1.f));
        }
    }
    reposition();
}

void LabelComponent::handleMove() { reposition(); }

sf::Vector2f LabelComponent::getRequisition() const {
    const sf::FloatRect bounds = text.getLocalBounds();
    return {bounds.position.x + bounds.size.x + 4.f, bounds.position.y + bounds.size.y};
}

void LabelComponent::reposition() {
    Label& owner                     = getOwnerAs<Label>();
    const RenderSettings& settings   = owner.renderSettings();
    const sf::Vector2f& parentOffset = owner.getLocalPosition();
    const sf::Vector2f size          = getRequisition();

    const sf::Vector2f localPos = RenderSettings::calculatePosition(
        settings.horizontalAlignment.value_or(RenderSettings::Center),
        settings.verticalAlignment.value_or(RenderSettings::Center),
        owner.getAcquisition(),
        size);
    const sf::Vector2f pos = localPos + parentOffset;

    text.getTransform().setPosition({pos.x, pos.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
