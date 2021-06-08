#include <BLIB/Interfaces/GUI/Renderers/Renderer.hpp>

#include <BLIB/Interfaces/GUI.hpp>
#include <Interfaces/GUI/Data/Font.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
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

} // namespace gui
} // namespace bl