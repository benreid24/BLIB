#include <BLIB/Interfaces/GUI/Elements/Separator.hpp>

namespace bl
{
namespace gui
{
Separator::Ptr Separator::create(Direction dir) { return Ptr(new Separator(dir)); }

Separator::Separator(Direction d)
: Element()
, dir(d) {
    if (dir == Horizontal)
        setExpandsWidth(true);
    else
        setExpandsHeight(true);
}

Separator::Direction Separator::getDirection() const { return dir; }

sf::Vector2i Separator::minimumRequisition() const {
    const int thick = renderSettings().outlineThickness.value_or(DefaultThickness);
    return {thick, thick};
}

void Separator::doRender(sf::RenderTarget& target, sf::RenderStates states,
                         const Renderer& renderer) const {
    renderer.renderSeparator(target, states, *this);
}

} // namespace gui
} // namespace bl