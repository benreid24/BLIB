#include <BLIB/GUI/Elements/Button.hpp>

#include <BLIB/GUI/Elements/Label.hpp>
#include <GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
Button::Ptr Button::create(const std::string& text, const std::string& group,
                           const std::string& id) {
    return Ptr(new Button(text, group, id));
}

Button::Button(const std::string& text, const std::string& group, const std::string& id)
: Element(group, id)
, text(text) {
    getSignal(Action::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&Button::settingsChanged, this));
}

void Button::setText(const std::string& t) {
    text = t;
    makeDirty();
}

const std::string& Button::getText() const { return text; }

sf::Vector2i Button::minimumRequisition() const {
    sf::Text renderText;
    bl::Resource<sf::Font>::Ref font = renderSettings().font.value_or(Font::get());
    if (font) renderText.setFont(*font);
    renderText.setString(text);
    renderText.setCharacterSize(
        renderSettings().characterSize.value_or(Label::DefaultFontSize));
    renderText.setStyle(renderSettings().style.value_or(sf::Text::Regular));
    return {static_cast<int>(renderText.getGlobalBounds().width +
                             renderText.getGlobalBounds().left * 2),
            static_cast<int>(renderText.getGlobalBounds().height +
                             renderText.getGlobalBounds().top * 2)};
}

void Button::settingsChanged() { makeDirty(); }

void Button::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      Renderer::Ptr renderer) const {
    renderer->renderButton(target, states, *this);
}

} // namespace gui
} // namespace bl