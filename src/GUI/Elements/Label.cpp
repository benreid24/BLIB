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
    settingsChanged();
}

void Label::setText(const std::string& t) {
    text = t;
    settingsChanged();
}

void Label::setFont(bl::Resource<sf::Font>::Ref f) {
    font = f;
    if (font) renderText.setFont(*font);
    settingsChanged();
}

Resource<sf::Font>::Ref Label::getFont() const { return font; }

void Label::render(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    renderer->renderLabel(target, *this);
}

sf::Vector2i Label::minimumRequisition() const {
    return {static_cast<int>(renderText.getGlobalBounds().width),
            static_cast<int>(renderText.getGlobalBounds().height)};
}

void Label::settingsChanged() {
    renderText.setString(text);
    renderText.setCharacterSize(renderSettings().characterSize.value_or(12));
    renderText.setStyle(renderSettings().style.value_or(sf::Text::Regular));
    if (renderText.getGlobalBounds().width > getAcquisition().width ||
        renderText.getGlobalBounds().height > getAcquisition().height)
        makeDirty();
}

} // namespace gui
} // namespace bl