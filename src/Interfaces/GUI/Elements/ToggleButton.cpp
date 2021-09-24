#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace gui
{
ToggleButton::ToggleButton(Element::Ptr c)
: Button(c)
, butsRendered(false)
, activeButton(Canvas::create(10, 10))
, inactiveButton(Canvas::create(10, 10))
, value(false) {
    setHorizontalAlignment(RenderSettings::Left);
    setExpandsWidth(true);
    getChild()->setHorizontalAlignment(RenderSettings::Left);
    getChild()->setExpandsWidth(true);
    getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) { onClick(); });
    getSignal(Event::AcquisitionChanged)
        .willAlwaysCall(std::bind(&ToggleButton::onAcquisition, this));
}

Canvas::Ptr ToggleButton::getVisibleButton() const { return value ? activeButton : inactiveButton; }

Canvas::Ptr ToggleButton::getHiddenButton() const { return value ? inactiveButton : activeButton; }

bool ToggleButton::getValue() const { return value; }

void ToggleButton::setValue(bool v) {
    if (v != value) {
        value = v;
        fireSignal(Event(Event::ValueChanged, value));
    }
}

void ToggleButton::setToggleButtonSize(const sf::Vector2i& size) {
    butsRendered = false;
    activeButton->resize(size.x, size.y);
    inactiveButton->resize(size.x, size.y);
    makeDirty();
}

bool ToggleButton::propagateEvent(const Event& event) {
    Button::propagateEvent(event);
    activeButton->processEvent(event);
    inactiveButton->processEvent(event);
    return false;
}

sf::Vector2i ToggleButton::minimumRequisition() const {
    const sf::Vector2i butReq   = activeButton->getRequisition();
    const sf::Vector2i childReq = getChild()->getRequisition();
    return {butReq.x + childReq.x + 12, std::max(butReq.y, childReq.y)};
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
    Packer::manuallyPackElement(getChild(),
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
    getChild()->render(target, states, renderer);
    if (value) { activeButton->render(target, states, renderer); }
    else {
        inactiveButton->render(target, states, renderer);
    }

    renderer.renderMouseoverOverlay(target, states, getChild().get());
}

void ToggleButton::onClick() {
    value = !value;
    fireSignal(Event(Event::ValueChanged, value));
}

} // namespace gui
} // namespace bl
