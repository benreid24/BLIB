#include <BLIB/Interfaces/GUI/Elements/CheckButton.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
CheckButton::Ptr CheckButton::create(const std::string& text) {
    return create(Label::create(text));
}

CheckButton::Ptr CheckButton::create(const Element::Ptr& child) {
    return Ptr(new CheckButton(child));
}

CheckButton::CheckButton(const Element::Ptr& child)
: ToggleButton(child) {}

rdr::Component* CheckButton::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<CheckButton>(*this, getWindowOrGuiParent());
}

} // namespace gui
} // namespace bl
