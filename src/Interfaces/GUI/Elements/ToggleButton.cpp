#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
ToggleButton::ToggleButton(const Element::Ptr& c)
: CompositeElement<3>()
, butsRendered(false)
, child(c)
, activeButton(Canvas::create(10, 10))
, inactiveButton(Canvas::create(10, 10))
, value(false) {
    setHorizontalAlignment(RenderSettings::Left);
    setExpandsWidth(true);
    child->setHorizontalAlignment(RenderSettings::Left);
    child->setExpandsWidth(true);
    getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) { onClick(); });

    Element* children[3] = {child.get(), activeButton.get(), inactiveButton.get()};
    registerChildren(children);
}

const Canvas::Ptr& ToggleButton::getVisibleButton() const {
    return value ? activeButton : inactiveButton;
}

const Canvas::Ptr& ToggleButton::getHiddenButton() const {
    return value ? inactiveButton : activeButton;
}

bool ToggleButton::getValue() const { return value; }

void ToggleButton::setValue(bool v) {
    if (v != value) {
        value = v;
        fireSignal(Event(Event::ValueChanged, value));
    }
}

void ToggleButton::setToggleButtonSize(const sf::Vector2f& size) {
    butsRendered = false;
    activeButton->resize(size.x, size.y);
    inactiveButton->resize(size.x, size.y);
    makeDirty();
}

bool ToggleButton::propagateEvent(const Event& event) {
    sendEventToChildren(event, false);
    return false;
}

sf::Vector2f ToggleButton::minimumRequisition() const {
    const sf::Vector2f butReq   = activeButton->getRequisition();
    const sf::Vector2f childReq = child->getRequisition();
    return {butReq.x + childReq.x + 12.f, std::max(butReq.y, childReq.y)};
}

void ToggleButton::onAcquisition() {
    Packer::manuallyPackElement(activeButton,
                                {getAcquisition().left,
                                 getAcquisition().top,
                                 activeButton->getRequisition().x,
                                 getAcquisition().height});
    Packer::manuallyPackElement(inactiveButton,
                                {getAcquisition().left,
                                 getAcquisition().top,
                                 activeButton->getRequisition().x,
                                 getAcquisition().height});
    Packer::manuallyPackElement(child,
                                {getAcquisition().left + activeButton->getRequisition().x + 2,
                                 getAcquisition().top,
                                 getAcquisition().width - activeButton->getRequisition().x - 2,
                                 getAcquisition().height});
}

void ToggleButton::doRender(sf::RenderTarget& target, sf::RenderStates states,
                            const Renderer& renderer) const {
    if (!butsRendered) {
        butsRendered = true;
        renderToggles(*activeButton, *inactiveButton, renderer);
    }
    child->render(target, states, renderer);
    if (value) { activeButton->render(target, states, renderer); }
    else {
        inactiveButton->render(target, states, renderer);
    }

    renderer.renderMouseoverOverlay(target, states, child.get());
}

void ToggleButton::onClick() {
    value = !value;
    fireSignal(Event(Event::ValueChanged, value));
}

} // namespace gui
} // namespace bl
