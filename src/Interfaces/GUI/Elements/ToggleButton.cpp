#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
ToggleButton::ToggleButton(const Element::Ptr& c)
: Button(c)
, value(false)
, toggleSize(-1.f) {
    setHorizontalAlignment(RenderSettings::Left);
    setExpandsWidth(true);
    c->setHorizontalAlignment(RenderSettings::Left);
    c->setExpandsWidth(true);
    getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) { onClick(); });
}

bool ToggleButton::getValue() const { return value; }

void ToggleButton::setValue(bool v) {
    if (v != value) {
        value = v;
        fireSignal(Event(Event::ValueChanged, value));
    }
}

sf::Vector2f ToggleButton::minimumRequisition() const {
    const sf::Vector2f butReq = Button::minimumRequisition();
    const float s             = getToggleSize();
    return {butReq.x + s * 1.2f, std::max(butReq.y, s)};
}

void ToggleButton::onClick() {
    value = !value;
    fireSignal(Event(Event::ValueChanged, value));
}

void ToggleButton::setToggleSize(float s) {
    toggleSize = s;
    makeDirty();
}

float ToggleButton::getToggleSize() const {
    if (toggleSize < 0.f) { toggleSize = Button::minimumRequisition().y * 0.8f; }
    return toggleSize;
}

} // namespace gui
} // namespace bl
