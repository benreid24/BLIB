#include <BLIB/GUI/Renderers/DefaultRenderer.hpp>

#include <BLIB/GUI.hpp>
#include <GUI/Data/Font.hpp>
#include <cmath>
#include <iostream>

namespace bl
{
namespace gui
{
namespace
{
RenderSettings getWindowDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(75, 75, 75);
    settings.outlineColor     = sf::Color(20, 20, 20);
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getTitlebarDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(95, 95, 95);
    settings.outlineColor     = sf::Color(20, 20, 20);
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getButtonDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(70, 70, 70);
    settings.outlineColor     = sf::Color::Black;
    settings.outlineThickness = Button::DefaultOutlineThickness;
    return settings;
}

RenderSettings getSliderDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(120, 120, 120);
    settings.outlineColor     = sf::Color(85, 85, 85);
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getContainerDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color::Transparent;
    settings.outlineColor     = sf::Color::Transparent;
    settings.outlineThickness = 0;
    return settings;
}

RenderSettings getImageDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color::Transparent;
    settings.outlineColor     = sf::Color::Transparent;
    settings.outlineThickness = 0;
    return settings;
}

RenderSettings getLabelDefaults() {
    RenderSettings settings;
    settings.characterSize       = Label::DefaultFontSize;
    settings.fillColor           = sf::Color::Black;
    settings.outlineColor        = sf::Color(0, 0, 0, 90);
    settings.outlineThickness    = 0;
    settings.style               = sf::Text::Regular;
    settings.horizontalAlignment = RenderSettings::Center;
    settings.verticalAlignment   = RenderSettings::Center;
    return settings;
}
} // namespace

DefaultRenderer::Ptr DefaultRenderer::create() { return Ptr(new DefaultRenderer()); }

void DefaultRenderer::renderCustom(sf::RenderTarget& target, sf::RenderStates states,
                                   const Element& element) const {
    std::cerr << "Error: renderCustom() called on default renderer. Use a custom renderer\n";
}

void DefaultRenderer::renderBox(sf::RenderTarget& target, sf::RenderStates states,
                                const Container& container) const {
    const RenderSettings settings        = getSettings(&container);
    static const RenderSettings defaults = getContainerDefaults();
    renderRectangle(target, states, container.getAcquisition(), settings, defaults);
}

void DefaultRenderer::renderButton(sf::RenderTarget& target, sf::RenderStates states,
                                   const Button& button) const {
    const RenderSettings settings        = getSettings(&button);
    static const RenderSettings defaults = getButtonDefaults();
    renderRectangle(target, states, button.getAcquisition(), settings, defaults);
}

void DefaultRenderer::renderMouseoverOverlay(sf::RenderTarget& target, sf::RenderStates states,
                                             const Element* element) const {
    sf::RectangleShape rect({static_cast<float>(element->getAcquisition().width),
                             static_cast<float>(element->getAcquisition().height)});
    rect.setPosition(element->getAcquisition().left, element->getAcquisition().top);
    if (element->mouseOver() || element->leftPressed()) {
        rect.setOutlineThickness(0);
        if (element->leftPressed())
            rect.setFillColor(sf::Color(30, 30, 30, 100));
        else
            rect.setFillColor(sf::Color(255, 255, 255, 100));
        target.draw(rect, states);
    }
}

void DefaultRenderer::renderSlider(sf::RenderTarget& target, sf::RenderStates states,
                                   const Slider& slider) const {
    const RenderSettings settings        = getSettings(&slider);
    static const RenderSettings defaults = getSliderDefaults();
    renderRectangle(target, states, slider.getAcquisition(), settings, defaults);
}

void DefaultRenderer::renderLabel(sf::RenderTarget& target, sf::RenderStates states,
                                  const Label& label) const {
    static const RenderSettings defaults = getLabelDefaults();
    renderText(target,
               states,
               label.getText(),
               label.getAcquisition(),
               getSettings(&label),
               defaults);
}

void DefaultRenderer::renderWindow(sf::RenderTarget& target, sf::RenderStates states,
                                   const Container* titlebar, const Window& window) const {
    const RenderSettings settings        = getSettings(&window);
    static const RenderSettings defaults = getWindowDefaults();

    const sf::FloatRect area = static_cast<sf::FloatRect>(window.getAcquisition());

    renderRectangle(target, states, window.getAcquisition(), settings, defaults);
    if (titlebar) {
        const RenderSettings settings        = getSettings(titlebar);
        static const RenderSettings defaults = getTitlebarDefaults();
        renderRectangle(target,
                        states,
                        {window.getAcquisition().left,
                         window.getAcquisition().top,
                         titlebar->getAcquisition().width,
                         titlebar->getAcquisition().height},
                        settings,
                        defaults);
    }
}

void DefaultRenderer::renderImage(sf::RenderTarget& target, sf::RenderStates states,
                                  const Element* e, const sf::Sprite& image) const {
    const RenderSettings settings        = getSettings(e);
    static const RenderSettings defaults = getImageDefaults();

    const sf::Vector2f size(image.getGlobalBounds().width, image.getGlobalBounds().height);
    const sf::Vector2f pos =
        calculatePosition(settings.horizontalAlignment.value_or(RenderSettings::Center),
                          settings.verticalAlignment.value_or(RenderSettings::Center),
                          e->getAcquisition(),
                          size);
    const sf::IntRect region(static_cast<sf::Vector2i>(pos), static_cast<sf::Vector2i>(size));

    renderRectangle(target, states, region, settings, defaults);
    states.transform.translate(pos);
    target.draw(image, states);
}

} // namespace gui
} // namespace bl