#include <BLIB/Interfaces/GUI/Renderers/Renderer.hpp>

#include <BLIB/Interfaces/GUI.hpp>
#include <Interfaces/GUI/Data/Font.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
Renderer::Renderer()
: tooltip(nullptr) {}

bool Renderer::viewValid(const sf::View& v) const {
    const sf::FloatRect& a = v.getViewport();
    if (a.width < 0.f) return false;
    if (a.height < 0.f) return false;
    if (a.left < 0.f) return false;
    if (a.top < 0.f) return false;
    if (a.width > 1.f) return false;
    if (a.height > 1.f) return false;
    return true;
}

const RenderSettings& Renderer::getSettings(const Element* e) const { return e->renderSettings(); }

void Renderer::setOriginalView(const sf::View& v) { ogView = v; }

const sf::View& Renderer::getOriginalView() const { return ogView; }

void Renderer::setTooltipToRender(const Element* e) const { tooltip = e; }

void Renderer::renderTooltip(sf::RenderTarget& target, sf::RenderStates states,
                             const sf::Vector2f& mousePos) const {
    if (tooltip && !tooltip->getTooltip().empty()) {
        renderTooltip(target, states, tooltip, mousePos);
        tooltip = nullptr;
    }
}

} // namespace gui
} // namespace bl
