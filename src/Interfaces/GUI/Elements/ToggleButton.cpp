#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
ToggleButton::ToggleButton(Element::Ptr c, const std::string& g, const std::string& i)
: Button(c, g, i)
, butsRendered(false)
, activeButton(Canvas::create(10, 10, g, i + "-checked"))
, inactiveButton(Canvas::create(10, 10, g, i + "-unchecked"))
, value(false) {
    updateButtons();
    setHorizontalAlignment(RenderSettings::Left);
    setExpandsWidth(true);
    getChild()->setHorizontalAlignment(RenderSettings::Left);
    getChild()->setExpandsWidth(true);
    getSignal(Action::LeftClicked).willAlwaysCall(std::bind(&ToggleButton::onClick, this));
    getSignal(Action::ValueChanged).willAlwaysCall(std::bind(&ToggleButton::updateButtons, this));
}

void ToggleButton::finishCreate() {
    Button::addChild();
    add(activeButton);
    add(inactiveButton);
}

Canvas::Ptr ToggleButton::getVisibleButton() const { return value ? activeButton : inactiveButton; }

Canvas::Ptr ToggleButton::getHiddenButton() const { return value ? inactiveButton : activeButton; }

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
    return {butReq.x + childReq.x + 2, std::max(butReq.y, childReq.y)};
}

void ToggleButton::onAcquisition() {
    Packer::manuallyPackElement(activeButton,
                                {0, 0, activeButton->getRequisition().x, getAcquisition().height});
    Packer::manuallyPackElement(inactiveButton,
                                {0, 0, activeButton->getRequisition().x, getAcquisition().height});
    Packer::manuallyPackElement(getChild(),
                                {activeButton->getRequisition().x + 2,
                                 0,
                                 getAcquisition().width - activeButton->getRequisition().x - 2,
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

    const sf::View oldView = target.getView();
    target.setView(computeView(target, getAcquisition()));
    // TODO - render mouse overlay
    target.setView(oldView);
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