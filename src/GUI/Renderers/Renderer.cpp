#include <BLIB/GUI.hpp>
#include <BLIB/GUI/Renderers/Renderer.hpp>

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

void Renderer::renderCustom(sf::RenderTarget& target, const Element& element) const {
    std::cerr << "Error: renderCustom() called on default renderer. Use a custom renderer\n";
}

void Renderer::renderContainer(sf::RenderTarget& target, const Container& container) const {
    const RenderSettings settings = getSettings(&container);
    const sf::FloatRect area      = static_cast<sf::FloatRect>(container.getAcquisition());
    sf::RectangleShape rect({area.width, area.height});
    rect.setPosition(area.left, area.top);
    rect.setFillColor(settings.fillColor.value_or(sf::Color::Transparent));
    rect.setOutlineThickness(settings.outlineThickness.value_or(0));
    rect.setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    target.draw(rect);
}

void Renderer::renderLabel(sf::RenderTarget& target, const Label& label) const {
    Resource<sf::Font>::Ref font = label.getFont();
    if (!font) {
        std::cerr << "Attempting to render a Label with no sf::Font\n";
        return;
    }

    const sf::FloatRect area      = static_cast<sf::FloatRect>(label.getAcquisition());
    const RenderSettings settings = getSettings(&label);

    sf::Text text;
    text.setFont(*font);
    text.setString(label.getText());
    text.setCharacterSize(settings.characterSize.value_or(12));
    text.setFillColor(settings.fillColor.value_or(sf::Color::Black));
    text.setOutlineColor(settings.outlineColor.value_or(sf::Color(0, 0, 0, 90)));
    text.setOutlineThickness(settings.outlineThickness.value_or(0));
    text.setStyle(settings.style.value_or(sf::Text::Regular));

    const sf::FloatRect size = text.getGlobalBounds();
    sf::Vector2f position;
    switch (settings.horizontalAlignment.value_or(RenderSettings::Center)) {
    case RenderSettings::Left:
        position.x = area.left;
        break;
    case RenderSettings::Right:
        position.x = area.left + area.width - size.width;
        break;
    case RenderSettings::Center:
    default:
        position.x = area.left + area.width / 2 - size.width / 2;
        break;
    }
    switch (settings.verticalAlignment.value_or(RenderSettings::Center)) {
    case RenderSettings::Top:
        position.y = area.top;
        break;
    case RenderSettings::Bottom:
        position.y = area.top + area.height - size.height;
        break;
    case RenderSettings::Center:
    default:
        position.y = area.top + area.height / 2 - size.height / 2;
        break;
    }

    text.setPosition(position);
    target.draw(text);
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