#include <BLIB/GUI/Elements/Label.hpp>

#include <GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
Label::Label(const std::string& text, const std::string& group, const std::string& id)
: Element(group, id)
, text(text)
, font(Font::get()) {
    if (font) renderText.setFont(*font);
    renderText.setCharacterSize(12);
    refresh();
}

void Label::setText(const std::string& t) {
    text = t;
    refresh();
}

void Label::setFont(bl::Resource<sf::Font>::Ref f) {
    font = f;
    if (font) renderText.setFont(*font);
    refresh();
}

void Label::setCharacterSize(unsigned int s) {
    renderText.setCharacterSize(s);
    settings.characterSize = s;
    refresh();
}

void Label::setColor(sf::Color fill, sf::Color outline) {
    renderText.setFillColor(fill);
    renderText.setOutlineColor(fill);
    settings.fillColor    = fill;
    settings.outlineColor = outline;
}

void Label::setOutlineThickness(unsigned int t) {
    renderText.setOutlineThickness(t);
    settings.outlineThickness = t;
    refresh();
}

void Label::setStyle(sf::Uint32 style) {
    renderText.setStyle(style);
    settings.style = style;
    refresh();
}

Resource<sf::Font>::Ref Label::getFont() const { return font; }

const RenderSettings& Label::renderSettings() const { return settings; }

void Label::render(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    renderer->renderLabel(target, *this);
}

sf::Vector2i Label::minimumRequisition() const {
    return {static_cast<int>(renderText.getGlobalBounds().width),
            static_cast<int>(renderText.getGlobalBounds().height)};
}

void Label::refresh() {
    renderText.setString(text);
    if (renderText.getGlobalBounds().width > getAcquisition().width ||
        renderText.getGlobalBounds().height > getAcquisition().height)
        makeDirty();
}

} // namespace gui
} // namespace bl