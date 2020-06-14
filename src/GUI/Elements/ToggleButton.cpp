#include <BLIB/GUI/Elements/ToggleButton.hpp>

#include <BLIB/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
ToggleButton::ToggleButton(Element::Ptr c, const std::string& g, const std::string& i)
: Button(c, g, i)
, butsRendered(false)
, value(false) {
    setToggleButtonSize({10, 10});
    updateButtons();
    getSignal(Action::LeftClicked).willAlwaysCall(std::bind(&ToggleButton::onClick, this));
    getSignal(Action::ValueChanged)
        .willAlwaysCall(std::bind(&ToggleButton::updateButtons, this));
}

void ToggleButton::finishCreate() {
    Button::addChild();
    add(activeButton);
    add(inactiveButton);
}

Canvas::Ptr ToggleButton::getVisibleButton() { return value ? activeButton : inactiveButton; }

Canvas::Ptr ToggleButton::getHiddenButton() { return value ? inactiveButton : activeButton; }

bool ToggleButton::getValue() const { return value; }

void ToggleButton::setValue(bool v) {
    if (v != value) {
        value = v;
        fireSignal(Action(Action::ValueChanged, value));
    }
}

void ToggleButton::setToggleButtonSize(const sf::Vector2i& size) {
    butsRendered = false;
    activeButton->resize(size.x, size.y);
    inactiveButton->resize(size.x, size.y);
    makeDirty();
}

bool ToggleButton::handleRawEvent(const RawEvent& event) {
    const RawEvent transformed = transformEvent(event);
    Button::handleRawEvent(event);
    activeButton->handleEvent(transformed);
    inactiveButton->handleEvent(transformed);
    return false;
}

sf::Vector2i ToggleButton::minimumRequisition() const {
    const sf::Vector2i butReq   = activeButton->getRequisition();
    const sf::Vector2i childReq = getChild()->getRequisition();
    return {butReq.x + childReq.x, std::max(butReq.y, childReq.y)};
}

void ToggleButton::onAcquisition() {
    Packer::manuallyPackElement(
        activeButton, {0, 0, activeButton->getRequisition().x, getAcquisition().height});
    Packer::manuallyPackElement(
        inactiveButton, {0, 0, activeButton->getRequisition().x, getAcquisition().height});
    Packer::manuallyPackElement(getChild(),
                                {activeButton->getRequisition().x,
                                 0,
                                 getAcquisition().width - activeButton->getRequisition().x,
                                 getAcquisition().height});
}

void ToggleButton::doRender(sf::RenderTarget& target, sf::RenderStates states,
                            const Renderer& renderer) const {
    if (!butsRendered) {
        butsRendered = true;
        renderToggles(*activeButton, *inactiveButton, renderer);
    }
    renderer.renderToggleButton(target, states, *this);
    renderChildren(target, states, renderer);
}

void ToggleButton::onClick() {
    value = !value;
    fireSignal(Action(Action::ValueChanged, value));
}

void ToggleButton::updateButtons() {
    activeButton->setVisible(value, false);
    inactiveButton->setVisible(!value, false);
}

} // namespace gui
} // namespace bl