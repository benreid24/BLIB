#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
Label::Ptr Label::create(const std::string& text, const std::string& group, const std::string& id) {
    return Ptr(new Label(text, group, id));
}

Label::Label(const std::string& text, const std::string& group, const std::string& id)
: Element(group, id)
, text(text) {
    getSignal(Action::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&Label::settingsChanged, this));
    settingsChanged();
}

void Label::setText(const std::string& t) {
    text = t;
    settingsChanged();
}

const std::string& Label::getText() const { return text; }

void Label::doRender(sf::RenderTarget& target, sf::RenderStates states,
                     const Renderer& renderer) const {
    renderer.renderLabel(target, states, *this);
}

sf::Vector2i Label::minimumRequisition() const {
    return {
        static_cast<int>(renderText.getGlobalBounds().width + renderText.getGlobalBounds().left),
        static_cast<int>(renderText.getGlobalBounds().height + renderText.getGlobalBounds().top)};
}

void Label::settingsChanged() {
    bl::resource::Resource<sf::Font>::Ref font = renderSettings().font.value_or(Font::get());
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