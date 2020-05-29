#include <BLIB/GUI/Renderers/Renderer.hpp>

#include <BLIB/GUI.hpp>
#include <GUI/Data/Font.hpp>
#include <cmath>
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

void Renderer::renderText(sf::RenderTarget& target, sf::RenderStates states,
                          const std::string& text, const sf::IntRect& acquisition,
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

    const sf::Vector2f size(sfText.getGlobalBounds().width + sfText.getGlobalBounds().left * 2,
                            sfText.getGlobalBounds().height +
                                sfText.getGlobalBounds().top * 2);
    sfText.setPosition(
        calculatePosition(settings.horizontalAlignment.value_or(RenderSettings::Center),
                          settings.verticalAlignment.value_or(RenderSettings::Center),
                          acquisition,
                          size));
    target.draw(sfText, states);
}

sf::Vector2f Renderer::calculatePosition(RenderSettings::Alignment horizontalAlignment,
                                         RenderSettings::Alignment verticalAlignment,
                                         const sf::IntRect& region,
                                         const sf::Vector2f& size) const {
    sf::Vector2f position;
    switch (horizontalAlignment) {
    case RenderSettings::Left:
        position.x = region.left;
        break;
    case RenderSettings::Right:
        position.x = region.left + region.width - size.x;
        break;
    case RenderSettings::Center:
    default:
        position.x = region.left + region.width / 2 - size.x / 2;
        break;
    }

    switch (verticalAlignment) {
    case RenderSettings::Top:
        position.y = region.top;
        break;
    case RenderSettings::Bottom:
        position.y = region.top + region.height - size.y;
        break;
    case RenderSettings::Center:
    default:
        position.y = region.top + region.height / 2 - size.y / 2;
        break;
    }

    return position;
}

void Renderer::renderCustom(sf::RenderTarget& target, sf::RenderStates states,
                            const Element& element) const {
    std::cerr << "Error: renderCustom() called on default renderer. Use a custom renderer\n";
}

void Renderer::renderContainer(sf::RenderTarget& target, sf::RenderStates states,
                               const Container& container) const {
    const RenderSettings settings = getSettings(&container);
    const sf::FloatRect area      = static_cast<sf::FloatRect>(container.getAcquisition());
    sf::RectangleShape rect({area.width, area.height});
    rect.setPosition(area.left, area.top);
    rect.setFillColor(settings.fillColor.value_or(sf::Color::Transparent));
    rect.setOutlineThickness(-settings.outlineThickness.value_or(0));
    rect.setOutlineColor(settings.outlineColor.value_or(sf::Color::Transparent));
    target.draw(rect, states);
}

void Renderer::renderButton(sf::RenderTarget& target, sf::RenderStates states,
                            const Button& button) const {
    const RenderSettings settings = getSettings(&button);
    sf::RectangleShape rect({static_cast<float>(button.getAcquisition().width),
                             static_cast<float>(button.getAcquisition().height)});
    rect.setPosition(button.getAcquisition().left, button.getAcquisition().top);
    rect.setFillColor(settings.fillColor.value_or(sf::Color(70, 70, 70)));
    rect.setOutlineColor(settings.outlineColor.value_or(sf::Color::Black));
    rect.setOutlineThickness(-settings.outlineThickness.value_or(2));
    target.draw(rect, states);

    renderText(target, states, button.getText(), button.getAcquisition(), settings);

    if (button.mouseOver() || button.leftPressed()) {
        rect.setOutlineThickness(0);
        if (button.leftPressed())
            rect.setFillColor(sf::Color(30, 30, 30, 100));
        else
            rect.setFillColor(sf::Color(255, 255, 255, 100));
        target.draw(rect, states);
    }
}

void Renderer::renderLabel(sf::RenderTarget& target, sf::RenderStates states,
                           const Label& label) const {
    renderText(
        target, states, label.getText(), label.getAcquisition(), label.renderSettings());
}

void Renderer::renderWindow(sf::RenderTarget& target, sf::RenderStates states,
                            const Container* titlebar, const Window& window) const {
    if (titlebar) {
        const RenderSettings settings = getSettings(titlebar);
        const sf::FloatRect area      = static_cast<sf::FloatRect>(titlebar->getAcquisition());
        sf::RectangleShape rect({area.width, area.height});
        rect.setPosition(area.left, area.top);
        rect.setFillColor(settings.fillColor.value_or(sf::Color(95, 95, 95)));
        rect.setOutlineThickness(-settings.outlineThickness.value_or(1));
        rect.setOutlineColor(settings.outlineColor.value_or(sf::Color(20, 20, 20)));
        target.draw(rect, states);
    }
    const RenderSettings settings = getSettings(&window);
    const sf::FloatRect area      = static_cast<sf::FloatRect>(window.getAcquisition());
    sf::RectangleShape rect({area.width, area.height});
    rect.setPosition(area.left, area.top);
    rect.setFillColor(settings.fillColor.value_or(sf::Color(75, 75, 75)));
    rect.setOutlineThickness(-settings.outlineThickness.value_or(1));
    rect.setOutlineColor(settings.outlineColor.value_or(sf::Color(20, 20, 20)));
    target.draw(rect, states);
}

} // namespace gui
} // namespace bl