#include <BLIB/GUI.hpp>

#include <GUI/Data/Font.hpp>
#include <sstream>

namespace bl
{
namespace gui
{
DebugRenderer::Ptr DebugRenderer::create(Renderer::Ptr renderer) {
    return Ptr(new DebugRenderer(renderer));
}

DebugRenderer::DebugRenderer(Renderer::Ptr renderer)
: underlying(renderer)
, showBounds(false)
, printGroups(false)
, printIds(false) {}

void DebugRenderer::showAcquisitions(bool show) { showBounds = show; }

void DebugRenderer::showGroups(bool show) { printGroups = show; }

void DebugRenderer::showIds(bool show) { printIds = show; }

void DebugRenderer::renderCustom(sf::RenderTarget& target, const Element& element) const {
    underlying->renderCustom(target, element);
    addInfo(target, element.getAcquisition(), element.group(), element.id());
}

void DebugRenderer::renderLabel(sf::RenderTarget& target, const Label& element) const {
    underlying->renderLabel(target, element);
    addInfo(target, element.getAcquisition(), element.group(), element.id());
}

void DebugRenderer::renderContainer(sf::RenderTarget& target, const Container& element) const {
    underlying->renderContainer(target, element);
    addInfo(target, element.getAcquisition(), element.group(), element.id());
}

void DebugRenderer::renderButton(sf::RenderTarget& target, const Button& element) const {
    underlying->renderButton(target, element);
    addInfo(target, element.getAcquisition(), element.group(), element.id());
}

void DebugRenderer::renderWindow(sf::RenderTarget& target, const Container& element) const {
    underlying->renderWindow(target, element);
    addInfo(target, element.getAcquisition(), element.group(), element.id());
}

void DebugRenderer::addInfo(sf::RenderTarget& target, const sf::IntRect& region,
                            const std::string& group, const std::string& id) const {
    if (showBounds) {
        sf::RectangleShape rect(sf::Vector2f(region.width, region.height));
        rect.setPosition(region.left, region.top);
        rect.setOutlineThickness(2);
        rect.setOutlineColor(sf::Color::Red);
        rect.setFillColor(sf::Color::Transparent);
        target.draw(rect);
    }
    if (printGroups || printIds) {
        std::stringstream ss;
        if (printGroups) ss << "g: " << group << "\n";
        if (printIds) ss << "i: " << id;

        sf::Text text;
        text.setString(ss.str());
        text.setPosition(region.left + 3, region.top + 3);
        text.setFillColor(sf::Color::Cyan);
        text.setOutlineColor(sf::Color::Magenta);
        text.setOutlineThickness(1.5);
        text.setCharacterSize(12);
        text.setFont(*Font::get());
        target.draw(text);
    }
}

} // namespace gui
} // namespace bl