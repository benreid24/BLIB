#include <BLIB/GUI/Renderers/Renderer.hpp>

#include <BLIB/GUI.hpp>
#include <GUI/Data/Font.hpp>
#include <iostream>

namespace bl
{
namespace gui
{
Renderer::Ptr Renderer::create() { return Ptr(new Renderer()); }

void Renderer::setGroupSettings(const std::string& group, const RenderSettings& settings) {
    groupSettings[group] = settings;
}

void Renderer::setIdSettings(const std::string& id, const RenderSettings& settings) {
    idSettings[id] = settings;
}

RenderSettings Renderer::getSettings(const Element* element) const {
    RenderSettings result;
    auto it = groupSettings.find(element->group());
    if (it != groupSettings.end()) result.merge(it->second);
    it = idSettings.find(element->id());
    if (it != idSettings.end()) result.merge(it->second);
    result.merge(element->renderSettings());
    return result;
}

void Renderer::renderText(sf::RenderTarget& target, const std::string& text,
                          const sf::IntRect& acquisition,
                          const RenderSettings& settings) const {
    Resource<sf::Font>::Ref font = settings.font.value_or(Font::get());
    if (!font) {
        std::cerr << "Attempting to render text with no sf::Font\n";
        return;
    }

    sf::Text sfText;
    sfText.setFont(*font);
    sfText.setString(text);
    sfText.setCharacterSize(settings.characterSize.value_or(Label::DefaultFontSize));
    sfText.setFillColor(settings.fillColor.value_or(sf::Color::Black));
    sfText.setOutlineColor(settings.outlineColor.value_or(sf::Color(0, 0, 0, 90)));
    sfText.setOutlineThickness(settings.outlineThickness.value_or(0));
    sfText.setStyle(settings.style.value_or(sf::Text::Regular));

    const sf::FloatRect size = sfText.getGlobalBounds();
    sf::Vector2f position;
    switch (settings.horizontalAlignment.value_or(RenderSettings::Center)) {
    case RenderSettings::Left:
        position.x = acquisition.left;
        break;
    case RenderSettings::Right:
        position.x = acquisition.left + acquisition.width - size.width;
        break;
    case RenderSettings::Center:
    default:
        position.x = acquisition.left + acquisition.width / 2 - size.width / 2;
        break;
    }
    switch (settings.verticalAlignment.value_or(RenderSettings::Center)) {
    case RenderSettings::Top:
        position.y = acquisition.top;
        break;
    case RenderSettings::Bottom:
        position.y = acquisition.top + acquisition.height - size.height;
        break;
    case RenderSettings::Center:
    default:
        position.y = acquisition.top + acquisition.height / 2 - size.height / 2;
        break;
    }

    std::cout << "Rendering (" << position.x << ", " << position.y << ")\n";
    sfText.setPosition(position);
    target.draw(sfText);
}

void Renderer::renderCustom(sf::RenderTarget& target, const Element& element) const {
    std::cerr << "Error: renderCustom() called on default renderer. Use a custom renderer\n";
}

void Renderer::renderContainer(sf::RenderTarget& target, const Container& container) const {
    if (!container.visible()) return;

    const RenderSettings settings = getSettings(&container);
    const sf::FloatRect area      = static_cast<sf::FloatRect>(container.getAcquisition());
    sf::RectangleShape rect({area.width, area.height});
    rect.setPosition(area.left, area.top);
    rect.setFillColor(settings.fillColor.value_or(sf::Color::Transparent));
    rect.setOutlineThickness(settings.outlineThickness.value_or(0));
    rect.setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    target.draw(rect);
}

void Renderer::renderButton(sf::RenderTarget& target, const Button& button) const {
    if (!button.visible()) return;

    sf::RectangleShape rect({static_cast<float>(button.getAcquisition().width),
                             static_cast<float>(button.getAcquisition().height)});
    rect.setPosition(button.getAcquisition().left, button.getAcquisition().top);
    rect.setFillColor(sf::Color(80, 80, 80));

    target.draw(rect);
    renderText(target, button.getText(), button.getAcquisition(), getSettings(&button));
    if (button.mouseOver()) {
        if (button.leftPressed())
            rect.setFillColor(sf::Color(30, 30, 30, 100));
        else
            rect.setFillColor(sf::Color(255, 255, 255, 100));
        target.draw(rect);
    }
}

void Renderer::renderLabel(sf::RenderTarget& target, const Label& label) const {
    if (!label.visible()) return;
    renderText(target, label.getText(), label.getAcquisition(), label.renderSettings());
}

void Renderer::renderTitlebar(sf::RenderTarget& target, const Container& titleBar,
                              const Label& title,
                              std::optional<const Element*> closeButton) const {
    sf::RectangleShape rect({static_cast<float>(titleBar.getAcquisition().width),
                             static_cast<float>(titleBar.getAcquisition().height)});
    rect.setFillColor(sf::Color(70, 70, 70));
    rect.setPosition(titleBar.getAcquisition().left, titleBar.getAcquisition().height);
    target.draw(rect);
    renderLabel(target, title);
    if (closeButton.has_value()) renderCustom(target, *closeButton.value()); // TODO - button
}

void Renderer::renderWindow(sf::RenderTarget& target, const Container& window) const {
    renderContainer(target, window);
}

} // namespace gui
} // namespace bl