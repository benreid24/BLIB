#include <BLIB/Interfaces/GUI/Elements/Separator.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

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

sf::Vector2f Separator::minimumRequisition() const {
    const float thick = renderSettings().outlineThickness.value_or(DefaultThickness);
    return {thick, thick};
}

rdr::Component* Separator::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Separator>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

} // namespace gui
} // namespace bl
