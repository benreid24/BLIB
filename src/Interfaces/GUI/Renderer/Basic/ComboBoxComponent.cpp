#include <BLIB/Interfaces/GUI/Renderer/Basic/ComboBoxComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/ComboBox.hpp>
#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
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
: Component(HighlightState::IgnoresMouse)
, enginePtr(nullptr)
, currentOverlay(nullptr) {}

void ComboBoxComponent::setVisible(bool v) { box.setHidden(!v); }

void ComboBoxComponent::onElementUpdated() {
    ComboBox& owner = getOwnerAs<ComboBox>();

    openBackground.scaleToSize({owner.getOptionRegion().width, owner.getOptionRegion().height});
    if (owner.isOpened()) {
        selectedOption.setHidden(true);
        openBackground.setHidden(false);
        if (optionsOutdated()) { updateOptions(); }
        openOptionBoxes.getTransform().setPosition({0.f, -owner.getScroll()});
        highlightMoused();
    }
    else {
        openBackground.setHidden(true);
        if (owner.getSelectedOption() >= 0) {
            selectedOption.setHidden(false);
            selectedOption.getSection().setString(owner.getSelectedOptionText());
            positionSelectedText();
        }
    }
}

void ComboBoxComponent::onRenderSettingChange() {
    ComboBox& owner                = getOwnerAs<ComboBox>();
    const RenderSettings& settings = owner.getRenderSettings();
    box.setFillColor(settings.fillColor.value_or(sf::Color(100, 100, 100)));
    box.setOutlineColor(settings.outlineColor.value_or(sf::Color::Black));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));

    configureText(selectedOption, settings);

    updateOptions();
    openBackground.scaleToSize({owner.getOptionRegion().width, owner.getOptionRegion().height});
    openBackground.getTransform().setPosition({0.f, box.getLocalBounds().height});
}

ecs::Entity ComboBoxComponent::getEntity() const { return box.entity(); }

void ComboBoxComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    enginePtr                      = &engine;
    ComboBox& owner                = getOwnerAs<ComboBox>();
    const RenderSettings& settings = owner.renderSettings();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});

    // arrow box + arrow
    arrowBox.create(
        engine, {owner.getAcquisition().height - BoxPad, owner.getAcquisition().height - BoxPad});
    arrowBox.setFillColor(sf::Color(90, 90, 90));
    arrowBox.setOutlineColor(sf::Color(45, 45, 45));
    arrowBox.setOutlineThickness(-1.f);
    arrowBox.setParent(box);
    arrow.create(engine, {glm::vec2{0.f, 0.f}, glm::vec2{100.f, 0.f}, glm::vec2{50.f, 100.f}});
    arrow.setFillColor(sf::Color::Black);
    arrow.getTransform().setOrigin({50.f, 50.f});
    arrow.getOverlayScaler().positionInParentSpace({0.5f, 0.5f});
    arrow.getOverlayScaler().scaleToSizePercent({0.4f, 0.4f});
    arrow.setParent(arrowBox);

    // selected text
    selectedOption.create(engine, *settings.font.value_or(Font::get()));
    selectedOption.setParent(box);

    // open state items
    openBackground.create(engine, {100.f, 100.f});
    openBackground.setFillColor(sf::Color(90, 90, 90));
    openBackground.getTransform().setDepth(-450.f);
    openBackground.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelf);
    openBackground.setParent(box);

    openOptionBoxes.create(engine, 256);
    openOptionBoxes.setParent(openBackground);
}

void ComboBoxComponent::doSceneAdd(rc::Overlay* overlay) {
    currentOverlay = overlay;

    box.addToScene(overlay, rc::UpdateSpeed::Static);
    arrowBox.addToScene(overlay, rc::UpdateSpeed::Static);
    arrow.addToScene(overlay, rc::UpdateSpeed::Static);
    selectedOption.addToScene(overlay, rc::UpdateSpeed::Static);

    openBackground.addToScene(overlay, rc::UpdateSpeed::Static);
    openOptionBoxes.addToScene(overlay, rc::UpdateSpeed::Static);
    if (optionsOutdated()) { updateOptions(); }
    for (auto& option : openOptions) { option.text.addToScene(overlay, rc::UpdateSpeed::Static); }
}

void ComboBoxComponent::doSceneRemove() {
    currentOverlay = nullptr;
    box.removeFromScene();
}

void ComboBoxComponent::handleAcquisition() {
    ComboBox& owner = getOwnerAs<ComboBox>();

    // background and arrow button
    box.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    arrowBox.setSize(
        {owner.getAcquisition().height - BoxPad, owner.getAcquisition().height - BoxPad});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
    arrowBox.getTransform().setPosition(
        {owner.getAcquisition().width - ComboBox::OptionPadding - arrowBox.getSize().x,
         ComboBox::OptionPadding});

    // closed text
    positionSelectedText();

    // open box and texts
    openBackground.scaleToSize({owner.getOptionRegion().width, owner.getOptionRegion().height});
    openBackground.getTransform().setPosition({0.f, owner.getAcquisition().height});
    if (optionsOutdated() ||
        (!openOptions.empty() &&
         (openOptions.front().background.getSize().x != owner.getOptionSize().x ||
          openOptions.front().background.getSize().y != owner.getOptionSize().y))) {
        updateOptions();
    }
}

void ComboBoxComponent::handleMove() {
    ComboBox& owner = getOwnerAs<ComboBox>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void ComboBoxComponent::configureText(gfx::Text& text, const RenderSettings& settings) {
    auto& sec = text.getSection();
    text.setFont(*settings.font.value_or(Font::get()));
    sec.setCharacterSize(settings.characterSize.value_or(Label::DefaultFontSize));
    sec.setFillColor(settings.secondaryFillColor.value_or(sf::Color::Black));
    sec.setOutlineColor(settings.secondaryOutlineColor.value_or(sf::Color::Transparent));
    sec.setOutlineThickness(settings.secondaryOutlineThickness.value_or(0));
    sec.setStyle(settings.style.value_or(sf::Text::Regular));
}

sf::Vector2f ComboBoxComponent::getRequisition() const {
    if (optionsOutdated()) { const_cast<ComboBoxComponent*>(this)->updateOptions(); }

    sf::Vector2f req(0.f, 0.f);
    for (const auto& option : openOptions) {
        const sf::FloatRect bounds = option.text.getLocalBounds();
        const float w              = bounds.left + bounds.width + BoxPad;
        const float h              = bounds.top + bounds.height + BoxPad;
        req.x                      = std::max(req.x, w);
        req.y                      = std::max(req.y, h);
    }

    return req;
}

void ComboBoxComponent::updateOptions() {
    ComboBox& owner                = getOwnerAs<ComboBox>();
    const RenderSettings& settings = owner.renderSettings();
    const glm::vec2 boxSize(owner.getOptionSize().x, owner.getOptionSize().y);
    const float outline = settings.outlineThickness.value_or(1.f);
    const sf::Vector2f innerSize(boxSize.x - outline * 2.f, boxSize.y - outline * 2.f);

    openOptions.resize(owner.getAllOptions().size());
    auto it = openOptions.begin();
    for (unsigned int i = 0; i < owner.getAllOptions().size(); ++i) {
        Option& o     = *(it++);
        const float y = static_cast<float>(i) * boxSize.y;

        if (!o.created) {
            o.created = true;
            o.background.create(*enginePtr, openOptionBoxes, boxSize);
            o.text.create(*enginePtr, *settings.font.value_or(Font::get()));
            o.text.setParent(openOptionBoxes);
            if (currentOverlay) { o.text.addToScene(currentOverlay, rc::UpdateSpeed::Static); }
        }

        o.text.getSection().setString(owner.getAllOptions()[i]);
        configureText(o.text, settings);
        const sf::Vector2f tpos =
            RenderSettings::calculatePosition(RenderSettings::Left,
                                              RenderSettings::Center,
                                              sf::FloatRect{sf::Vector2f{0.f, y}, innerSize},
                                              {o.text.getLocalSize().x, o.text.getLocalSize().y});
        o.text.getTransform().setPosition({tpos.x, tpos.y + y});

        o.background.setSize(boxSize);
        if (static_cast<int>(i) == owner.getMousedOption()) {
            o.background.setFillColor(settings.secondaryFillColor.value_or(sf::Color(80, 80, 250)));
        }
        else { o.background.setFillColor(settings.fillColor.value_or(sf::Color(100, 100, 100))); }
        o.background.setOutlineColor(settings.outlineColor.value_or(sf::Color::Black));
        o.background.setOutlineThickness(-outline);
        o.background.getLocalTransform().setPosition({0.f, y});
    }
}

void ComboBoxComponent::highlightMoused() {
    ComboBox& owner                = getOwnerAs<ComboBox>();
    const RenderSettings& settings = owner.renderSettings();

    auto it = openOptions.begin();
    for (unsigned int i = 0; i < owner.getAllOptions().size(); ++i) {
        Option& o = *(it++);

        if (static_cast<int>(i) == owner.getMousedOption()) {
            o.background.setFillColor(settings.secondaryFillColor.value_or(sf::Color(80, 80, 250)));
        }
        else { o.background.setFillColor(settings.fillColor.value_or(sf::Color(100, 100, 100))); }
    }
}

bool ComboBoxComponent::optionsOutdated() const {
    const ComboBox& owner = getOwnerAs<ComboBox>();
    if (owner.getAllOptions().size() != openOptions.size()) { return true; }
    auto it = openOptions.begin();
    for (unsigned int i = 0; i < openOptions.size(); ++i) {
        const auto& option      = *(it++);
        const std::string& text = owner.getAllOptions()[i];
        if (!option.created || option.text.getSection().getString() != text) { return true; }
    }
    return false;
}

void ComboBoxComponent::positionSelectedText() {
    ComboBox& owner          = getOwnerAs<ComboBox>();
    const glm::vec2 textSize = selectedOption.getLocalSize();
    const sf::Vector2f pos   = RenderSettings::calculatePosition(RenderSettings::Left,
                                                               RenderSettings::Center,
                                                               owner.getAcquisition(),
                                                                 {textSize.x, textSize.y});
    selectedOption.getTransform().setPosition({pos.x, pos.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
