#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
ToggleButton::ToggleButton(const Element::Ptr& c)
: CompositeElement()
, child(c)
, value(false)
, toggleSize(-1.f) {
    Element* tmp[1] = {c.get()};
    registerChildren(tmp);

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
        if (getComponent()) { getComponent()->onElementUpdated(); }
    }
}

sf::Vector2f ToggleButton::minimumRequisition() const {
    const sf::Vector2f butReq = child->getRequisition();
    const float s             = getToggleSize();
    return {butReq.x + s * 1.2f, std::max(butReq.y, s)};
}

void ToggleButton::onClick() {
    value = !value;
    fireSignal(Event(Event::ValueChanged, value));
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void ToggleButton::setToggleSize(float s) {
    toggleSize = s;
    makeDirty();
    if (getComponent()) { getComponent()->onRenderSettingChange(); }
}

float ToggleButton::getToggleSize() const {
    if (toggleSize <= 0.f) { toggleSize = child->getRequisition().y * 0.8f; }
    return toggleSize;
}

void ToggleButton::onAcquisition() {
    const float s = getToggleSize() * 1.2f;
    Packer::manuallyPackElement(child,
                                sf::FloatRect(sf::Vector2f(getAcquisition().position.x + s,
                                                            getAcquisition().position.y),
                                              sf::Vector2f(getAcquisition().size.x - s,
                                                            getAcquisition().size.y)));
}

bool ToggleButton::propagateEvent(const Event& event) {
    sendEventToChildren(event);
    return false; // so the button can have proper mouseover and click events
}

} // namespace gui
} // namespace bl
