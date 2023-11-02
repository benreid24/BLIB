#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
Label::Ptr Label::create(const std::string& text) { return Ptr(new Label(text)); }

Label::Label(const std::string& text)
: Element()
, text(text) {
    getSignal(Event::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&Label::settingsChanged, this));
    settingsChanged();
}

void Label::setText(const std::string& t) {
    text = t;
    settingsChanged();
}

const std::string& Label::getText() const { return text; }

rdr::Component* Label::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Label>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

sf::Vector2f Label::minimumRequisition() const {
    return {renderText.getGlobalBounds().width + renderText.getGlobalBounds().left,
            renderText.getGlobalBounds().height + renderText.getGlobalBounds().top};
}

void Label::settingsChanged() {
    bl::resource::Ref<sf::Font> font = renderSettings().font.value_or(Font::get());
    if (font) renderText.setFont(*font);
    renderText.setString(text);
    renderText.setCharacterSize(renderSettings().characterSize.value_or(DefaultFontSize));
    renderText.setStyle(renderSettings().style.value_or(sf::Text::Regular));
    if (renderText.getGlobalBounds().width > getAcquisition().width ||
        renderText.getGlobalBounds().height > getAcquisition().height)
        makeDirty();
}

} // namespace gui
} // namespace bl
