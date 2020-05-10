#include <BLIB/GUI/Elements/Button.hpp>

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
, text(text) {}

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
    renderText.setCharacterSize(renderSettings().characterSize.value_or(12));
    renderText.setStyle(renderSettings().style.value_or(sf::Text::Regular));
    return {static_cast<int>(renderText.getGlobalBounds().width),
            static_cast<int>(renderText.getGlobalBounds().height)};
}

void Button::settingsChanged() { makeDirty(); }

void Button::doRender(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    renderer->renderButton(target, *this);
}

} // namespace gui
} // namespace bl