#include <BLIB/Interfaces/GUI/Elements/Button.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr float ChildPadding = 4.f;
}

Button::Ptr Button::create(const std::string& text) { return Ptr(new Button(Label::create(text))); }

Button::Ptr Button::create(const Element::Ptr& e) { return Ptr(new Button(e)); }

Button::Button(const Element::Ptr& child)
: CompositeElement<1>()
, child(child) {
    Element* tmp[1] = {child.get()};
    registerChildren(tmp);
}

const Element::Ptr& Button::getChild() const { return child; }

sf::Vector2f Button::minimumRequisition() const {
    sf::Vector2f area = child->getRequisition();
    const float buffer =
        renderSettings().outlineThickness.value_or(DefaultOutlineThickness) + ChildPadding;
    area.x += buffer * 2.f;
    area.y += buffer * 2.f;
    return area;
}

void Button::onAcquisition() {
    const float buffer =
        renderSettings().outlineThickness.value_or(DefaultOutlineThickness) + ChildPadding;
    Packer::manuallyPackElement(child,
                                {getAcquisition().left + buffer,
                                 getAcquisition().top + buffer,
                                 getAcquisition().width - buffer * 2.f,
                                 getAcquisition().height - buffer * 2.f});
}

bool Button::propagateEvent(const Event& event) {
    sendEventToChildren(event);
    return false; // so the button can have proper mouseover and click events
}

rdr::Component* Button::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Button>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

} // namespace gui
} // namespace bl
