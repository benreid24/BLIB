#include <BLIB/Interfaces/GUI.hpp>

#include <Interfaces/GUI/Data/Font.hpp>
#include <sstream>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace gui
{
DebugRenderer::Ptr DebugRenderer::create(Renderer::Ptr renderer) {
    return Ptr(new DebugRenderer(renderer));
}

DebugRenderer::DebugRenderer(Renderer::Ptr renderer)
: underlying(renderer)
, showBounds(false) {}

void DebugRenderer::showAcquisitions(bool show) { showBounds = show; }

void DebugRenderer::renderLabel(sf::RenderTarget& target, sf::RenderStates states,
                                const Label& element) const {
    underlying->renderLabel(target, states, element);
    addInfo(target, states, element.getAcquisition());
}

void DebugRenderer::renderBox(sf::RenderTarget& target, sf::RenderStates states,
                              const Element& element) const {
    underlying->renderBox(target, states, element);
    addInfo(target, states, element.getAcquisition());
}

void DebugRenderer::renderImage(sf::RenderTarget& target, sf::RenderStates states,
                                const Element* element, const sf::Sprite& image) const {
    underlying->renderImage(target, states, element, image);
    addInfo(target, states, element->getAcquisition());
}

void DebugRenderer::renderButton(sf::RenderTarget& target, sf::RenderStates states,
                                 const Button& element) const {
    underlying->renderButton(target, states, element);
    addInfo(target, states, element.getAcquisition());
}

void DebugRenderer::renderComboBox(sf::RenderTarget& target, sf::RenderStates states,
                                   const ComboBox& box) const {
    underlying->renderComboBox(target, states, box);
    addInfo(target, states, box.getAcquisition());
}

void DebugRenderer::renderComboBoxDropdownBoxes(sf::RenderTarget& target, sf::RenderStates states,
                                                const ComboBox& box, const sf::Vector2f& optionSize,
                                                unsigned int optionCount,
                                                unsigned int mousedOption) const {
    underlying->renderComboBoxDropdownBoxes(
        target, states, box, optionSize, optionCount, mousedOption);
}

void DebugRenderer::renderComboBoxDropdownArrow(sf::RenderTexture& texture) const {
    underlying->renderComboBoxDropdownArrow(texture);
}

void DebugRenderer::renderMouseoverOverlay(sf::RenderTarget& target, sf::RenderStates states,
                                           const Element* element) const {
    underlying->renderMouseoverOverlay(target, states, element);
}

void DebugRenderer::renderNotebookTabs(sf::RenderTarget& target, sf::RenderStates states,
                                       const Notebook& nb, float scroll) const {
    underlying->renderNotebookTabs(target, states, nb, scroll);
    addInfo(target, states, nb.getAcquisition());
}

void DebugRenderer::renderProgressBar(sf::RenderTarget& target, sf::RenderStates states,
                                      const ProgressBar& bar) const {
    underlying->renderProgressBar(target, states, bar);
    addInfo(target, states, bar.getAcquisition());
}

void DebugRenderer::renderSeparator(sf::RenderTarget& target, sf::RenderStates states,
                                    const Separator& sep) const {
    underlying->renderSeparator(target, states, sep);
    addInfo(target, states, sep.getAcquisition());
}

void DebugRenderer::renderSlider(sf::RenderTarget& target, sf::RenderStates states,
                                 const Slider& slider) const {
    underlying->renderSlider(target, states, slider);
    addInfo(target, states, slider.getAcquisition());
}

void DebugRenderer::renderSliderButton(sf::RenderTexture& texture, bool hor,
                                       bool increasing) const {
    underlying->renderSliderButton(texture, hor, increasing);
}

void DebugRenderer::renderTextEntry(sf::RenderTarget& target, sf::RenderStates states,
                                    const TextEntry& entry) const {
    underlying->renderTextEntry(target, states, entry);
    addInfo(target, states, entry.getAcquisition());
}

void DebugRenderer::renderToggleCheckButton(sf::RenderTexture& texture, bool active) const {
    underlying->renderToggleCheckButton(texture, active);
}

void DebugRenderer::renderToggleRadioButton(sf::RenderTexture& texture, bool active) const {
    underlying->renderToggleRadioButton(texture, active);
}

void DebugRenderer::renderWindow(sf::RenderTarget& target, sf::RenderStates states,
                                 const Container* titlebar, const Window& window) const {
    underlying->renderWindow(target, states, titlebar, window);
    addInfo(target, states, window.getAcquisition());
}

void DebugRenderer::addInfo(sf::RenderTarget& target, sf::RenderStates states,
                            const sf::FloatRect& region) const {
    if (showBounds) {
        sf::RectangleShape rect(sf::Vector2f(region.width, region.height));
        rect.setPosition(region.left, region.top);
        rect.setOutlineThickness(-2.f);
        rect.setOutlineColor(sf::Color::Red);
        rect.setFillColor(sf::Color::Transparent);
        target.draw(rect, states);
    }
}

void DebugRenderer::renderTooltip(sf::RenderTarget& target, sf::RenderStates states,
                                  const Element* e, const sf::Vector2f& mousePos) const {
    underlying->renderTooltip(target, states, e, mousePos);
}

} // namespace gui
} // namespace bl
