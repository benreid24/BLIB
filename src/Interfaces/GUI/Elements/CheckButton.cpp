#include <BLIB/Interfaces/GUI/Elements/CheckButton.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

namespace bl
{
namespace gui
{
CheckButton::Ptr CheckButton::create(const std::string& text) {
    return create(Label::create(text));
}

CheckButton::Ptr CheckButton::create(Element::Ptr child) { return Ptr(new CheckButton(child)); }

CheckButton::CheckButton(Element::Ptr child)
: ToggleButton(child) {}

void CheckButton::renderToggles(Canvas& activeBut, Canvas& inactiveBut,
                                const Renderer& renderer) const {
    renderer.renderToggleCheckButton(activeBut.getTexture(), true);
    renderer.renderToggleCheckButton(inactiveBut.getTexture(), false);
}

} // namespace gui
} // namespace bl
