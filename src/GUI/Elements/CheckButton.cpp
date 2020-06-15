#include <BLIB/GUI/Elements/CheckButton.hpp>

#include <BLIB/GUI/Elements/Label.hpp>

namespace bl
{
namespace gui
{
CheckButton::Ptr CheckButton::create(const std::string& text, const std::string& group,
                                     const std::string& id) {
    return create(Label::create(text, group, id + "-label"), group, id);
}

CheckButton::Ptr CheckButton::create(Element::Ptr child, const std::string& group,
                                     const std::string& id) {
    Ptr but(new CheckButton(child, group, id));
    but->finishCreate();
    return but;
}

CheckButton::CheckButton(Element::Ptr child, const std::string& group, const std::string& id)
: ToggleButton(child, group, id) {}

void CheckButton::renderToggles(Canvas& activeBut, Canvas& inactiveBut,
                                const Renderer& renderer) const {
    renderer.renderToggleCheckButton(activeBut.getTexture(), true);
    renderer.renderToggleCheckButton(inactiveBut.getTexture(), false);
}

} // namespace gui
} // namespace bl