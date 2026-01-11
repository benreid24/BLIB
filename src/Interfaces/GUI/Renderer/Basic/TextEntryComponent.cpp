#include <BLIB/Interfaces/GUI/Renderer/Basic/TextEntryComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Elements/TextEntry.hpp>
#include <BLIB/Logging.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
namespace
{
constexpr float CaratWidth = 1.5f;
}

TextEntryComponent::TextEntryComponent()
: TextEntryComponentBase(HighlightState::IgnoresMouse) {}

void TextEntryComponent::setVisible(bool v) { box.setHidden(!v); }

void TextEntryComponent::onElementUpdated() {
    TextEntry& owner = getOwnerAs<TextEntry>();
    text.getSection().setString(owner.getInput());
    positionItems();
}

void TextEntryComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    const rc::Color caratCol       = settings.outlineColor.value_or(sf::Color::Black);

    box.setFillColor(settings.fillColor.value_or(sf::Color::White));
    box.setOutlineColor(caratCol);
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));

    auto& sec                   = text.getSection();
    const sf::VulkanFont& font  = *settings.font.value_or(Font::get());
    const unsigned int charSize = settings.characterSize.value_or(Label::DefaultFontSize);
    text.setFont(font);
    sec.setFillColor(settings.secondaryFillColor.value_or(sf::Color::Black));
    sec.setOutlineColor(settings.secondaryOutlineColor.value_or(sf::Color::Transparent));
    sec.setOutlineThickness(settings.secondaryOutlineThickness.value_or(0.f));
    sec.setCharacterSize(charSize);
    sec.setStyle(settings.style.value_or(sf::Text::Regular));

    carat.setFillColor(caratCol);
    carat.scaleToSize({CaratWidth, font.getLineSpacing(charSize)});

    positionItems();
}

ecs::Entity TextEntryComponent::getEntity() const { return box.entity(); }

unsigned int TextEntryComponent::findCursorPosition(const sf::Vector2f& overlayPos) const {
    const auto result = text.findCharacterAtPosition({overlayPos.x, overlayPos.y});
    if (result.found && result.sectionIndex == 0) { return result.characterIndex; }

    // determine clicked line and if at start or end
    const TextEntry& owner = getOwnerAs<TextEntry>();
    const glm::vec2 gp     = text.getTransform().getGlobalPosition();
    const sf::Vector2f lp  = overlayPos - sf::Vector2f(gp.x, gp.y);
    const float lineHeight = text.getFont().getLineSpacing(text.getSection().getCharacterSize());

    if (lp.y < 0.f) { return 0; }
    if (lp.y > lineHeight * static_cast<float>(owner.getLineCount())) {
        return owner.getInput().size();
    }

    const unsigned int line = std::floor(lp.y / lineHeight) + 1.f;
    if (lp.x < owner.getAcquisition().size.x * 0.5f) { return owner.getLineStartIndex(line); }
    else { return owner.getLineEndIndex(line); }
}

void TextEntryComponent::onCaratStateUpdate() {
    TextEntry& owner = getOwnerAs<TextEntry>();
    if (owner.cursorVisible()) {
        carat.setHidden(false);
        carat.flash(TextEntry::CursorFlashPeriod, TextEntry::CursorFlashPeriod);
        positionItems();
    }
    else {
        carat.stopFlashing();
        carat.setHidden(true);
    }
}

void TextEntryComponent::resetCaratFlash() {
    TextEntry& owner = getOwnerAs<TextEntry>();
    if (owner.cursorVisible()) { carat.resetFlash(); }
}

void TextEntryComponent::doCreate(engine::World& world, rdr::Renderer&) {
Element& owner = getOwnerAs<Element>();
box.create(world, {owner.getAcquisition().size.x, owner.getAcquisition().size.y});
box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);

    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    text.create(world, *settings.font.value_or(Font::get()));
    text.setParent(box);

    carat.create(world, {10.f, 40.f});
    carat.setParent(text);
    carat.setHidden(true);
}

void TextEntryComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
    text.addToScene(overlay, rc::UpdateSpeed::Static);
    carat.addToScene(overlay, rc::UpdateSpeed::Static);
}

void TextEntryComponent::doSceneRemove() { box.removeFromScene(); }

void TextEntryComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    box.setSize({owner.getAcquisition().size.x, owner.getAcquisition().size.y});
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
    positionItems();
}

void TextEntryComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    box.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
}

void TextEntryComponent::positionItems() {
    constexpr float Padding = 2.f;

    TextEntry& owner           = getOwnerAs<TextEntry>();
    const sf::FloatRect& acq   = owner.getAcquisition();
    const sf::FloatRect bounds = text.getLocalBounds();
    const glm::vec2 cpos       = text.findCharacterPosition(0, owner.getCursorPosition());

    glm::vec2 textOffset(0.f, 0.f);
    if ((bounds.size.x + bounds.position.x) > acq.size.x || (bounds.position.y + bounds.size.y) > acq.size.y) {
        const glm::vec2 gcpos = cpos + text.getTransform().getLocalPosition();
        const sf::Glyph& g    = text.getFont().getGlyph(
            text.getSection().getString()[owner.getCursorPosition()],
            text.getSection().getCharacterSize(),
            (text.getSection().getStyle() & sf::Text::Style::Bold) == sf::Text::Style::Bold,
            text.getSection().getOutlineThickness());

        const float right = acq.size.x - Padding * 2.f;
        if (gcpos.x < 0.f) { textOffset.x = -cpos.x; }
        else if (cpos.x > right) {
            textOffset.x = right - cpos.x - g.bounds.size.x - g.advance - CaratWidth;
        }

        const float lineHeight =
            text.getFont().getLineSpacing(text.getSection().getCharacterSize());
        const float y      = static_cast<float>(owner.getCurrentLine()) * lineHeight;
        const float by     = y + lineHeight;
        const float bottom = acq.size.y - Padding * 2.f;
        if (y < 0.f) { textOffset.y = -y; }
        else if (by > bottom) { textOffset.y = bottom - by; }
    }

    text.getTransform().setPosition(glm::vec2{Padding, Padding} + textOffset);
    carat.getTransform().setPosition(cpos);
}

} // namespace defcoms
} // namespace gui
} // namespace bl
