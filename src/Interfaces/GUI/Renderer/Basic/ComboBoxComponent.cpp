#include <BLIB/Interfaces/GUI/Renderer/Basic/ComboBoxComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/ComboBox.hpp>
#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Render/Primitives/Color.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
namespace
{
constexpr float BoxPad = ComboBox::OptionPadding * 2.f;
}

ComboBoxComponent::ComboBoxComponent()
: Component(HighlightState::IgnoresMouse) {}

void ComboBoxComponent::setVisible(bool v) { box.setHidden(!v); }

void ComboBoxComponent::onElementUpdated() {
    ComboBox& owner = getOwnerAs<ComboBox>();

    if (owner.isOpened()) {
        selectedOption.setHidden(true);
        // TODO - show open items
        // TODO - update highlight color for moused item, if any
        // TODO - update scroll for open items
    }
    else {
        // TODO - hide open items
        if (owner.getSelectedOption() >= 0) {
            selectedOption.setHidden(false);
            selectedOption.getSection().setString(owner.getSelectedOptionText());
        }
    }
}

void ComboBoxComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<ComboBox>().getRenderSettings();
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color(100, 100, 100))));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Black)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));

    configureText(selectedOption, settings);
    // TODO - open items
}

ecs::Entity ComboBoxComponent::getEntity() const { return box.entity(); }

void ComboBoxComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    ComboBox& owner                = getOwnerAs<ComboBox>();
    const RenderSettings& settings = owner.renderSettings();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});

    // arrow box + arrow
    arrowBox.create(
        engine, {owner.getAcquisition().height - BoxPad, owner.getAcquisition().height - BoxPad});
    arrowBox.setFillColor(sfcol(sf::Color(90, 90, 90)));
    arrowBox.setOutlineColor(sfcol(sf::Color(45, 45, 45)));
    arrowBox.setOutlineThickness(-1.f);
    arrowBox.setParent(box);
    arrow.create(engine, {glm::vec2{0.f, 0.f}, glm::vec2{100.f, 0.f}, glm::vec2{50.f, 100.f}});
    arrow.setFillColor(sfcol(sf::Color::Black));
    arrow.getTransform().setOrigin({50.f, 50.f});
    arrow.getOverlayScaler().positionInParentSpace({0.5f, 0.5f});
    arrow.getOverlayScaler().scaleToSizePercent({0.4f, 0.4f});
    arrow.setParent(arrowBox);

    // selected text
    selectedOption.create(engine, *settings.font.value_or(Font::get()));
    selectedOption.setParent(box);

    // TODO - open items
}

void ComboBoxComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
    arrowBox.addToScene(overlay, rc::UpdateSpeed::Static);
    arrow.addToScene(overlay, rc::UpdateSpeed::Static);
    selectedOption.addToScene(overlay, rc::UpdateSpeed::Static);

    // TODO - add open items
}

void ComboBoxComponent::doSceneRemove() { box.removeFromScene(); }

void ComboBoxComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                          const sf::Vector2f& size) {
    // background and arrow button
    box.setSize({size.x, size.y});
    arrowBox.setSize({size.y - BoxPad, size.y - BoxPad});
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
    arrowBox.getTransform().setPosition(
        {size.x - ComboBox::OptionPadding - arrowBox.getSize().x, ComboBox::OptionPadding});

    // closed text
    const glm::vec2 textSize = selectedOption.getLocalSize();
    const sf::Vector2f pos =
        RenderSettings::calculatePosition(RenderSettings::Left,
                                          RenderSettings::Center,
                                          getOwnerAs<ComboBox>().getAcquisition(),
                                          {textSize.x, textSize.y});
    selectedOption.getTransform().setPosition({pos.x, pos.y});

    // open box and texts
    // TODO
}

void ComboBoxComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void ComboBoxComponent::configureText(gfx::Text& text, const RenderSettings& settings) {
    auto& sec = text.getSection();
    text.setFont(*settings.font.value_or(Font::get()));
    sec.setCharacterSize(settings.characterSize.value_or(Label::DefaultFontSize));
    sec.setFillColor(sfcol(settings.secondaryFillColor.value_or(sf::Color::Black)));
    sec.setOutlineColor(sfcol(settings.secondaryOutlineColor.value_or(sf::Color::Transparent)));
    sec.setOutlineThickness(settings.secondaryOutlineThickness.value_or(0));
    sec.setStyle(settings.style.value_or(sf::Text::Regular));
}

sf::Vector2f ComboBoxComponent::getRequisition() const {
    const ComboBox& owner          = getOwnerAs<ComboBox>();
    const RenderSettings& settings = owner.renderSettings();
    const float fontSize =
        static_cast<float>(settings.characterSize.value_or(Label::DefaultFontSize));

    sf::Vector2f req(0.f, fontSize);

    // TODO - actually go through options
    for (const auto& option : owner.getAllOptions()) {
        const float w = option.size() * fontSize + BoxPad;
        req.x         = std::max(req.x, w);
    }

    return req;
}

} // namespace defcoms
} // namespace gui
} // namespace bl
