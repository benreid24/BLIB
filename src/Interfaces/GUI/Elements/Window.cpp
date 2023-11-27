#include <BLIB/Interfaces/GUI/Elements/Window.hpp>

#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
namespace
{
inline bool hasStyle(Window::Style style, Window::Style check) { return (style & check) == check; }
} // namespace

Window::Ptr Window::create(const Packer::Ptr& packer, const std::string& titleText, Style style,
                           const sf::Vector2f& position) {
    return Ptr(new Window(packer, titleText, style, position));
}

Window::Window(const Packer::Ptr& packer, const std::string& titleText, Style style,
               const sf::Vector2f& position)
: CompositeElement<2>()
, moveable(hasStyle(style, Moveable))
, titlebarHeight(22.f)
, elementArea(Box::create(packer)) {
    using namespace std::placeholders;
    const Signal::Callback dragCb   = std::bind(&Window::handleDrag, this, _1);
    const Signal::Callback activeCb = std::bind(&Window::moveToTop, this);

    titlebar = Box::create(
        LinePacker::create(LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::LeftAlign));
    titlebar->setColor(sf::Color(95, 95, 95), sf::Color(20, 20, 20));
    titlebar->setOutlineThickness(1.f);
    if (hasStyle(style, Titlebar)) {
        // Create titlebar containers
        leftTitleSide  = Box::create(LinePacker::create(LinePacker::Horizontal));
        rightTitleSide = Box::create(LinePacker::create(
            LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::RightAlign));
        rightTitleSide->setRequisition({titlebarHeight, titlebarHeight});
        rightTitleSide->setOutlineThickness(0.f);
        leftTitleSide->setOutlineThickness(0.f);
        titlebar->pack(leftTitleSide, true, true);
        titlebar->pack(rightTitleSide);
        titlebar->setExpandsWidth(true);
        titlebar->setExpandsHeight(true);

        // Title text
        title = Label::create(titleText);
        title->setCharacterSize(18);
        title->setHorizontalAlignment(RenderSettings::Left);
        leftTitleSide->pack(title, true, true);

        // Setup drag action
        getSignal(Event::Dragged).willAlwaysCall(dragCb);
        title->getSignal(Event::Dragged).willAlwaysCall(dragCb);
        title->getSignal(Event::LeftMousePressed).willAlwaysCall(activeCb);
        leftTitleSide->getSignal(Event::Dragged).willAlwaysCall(dragCb);
        leftTitleSide->getSignal(Event::LeftMousePressed).willAlwaysCall(activeCb);
        rightTitleSide->getSignal(Event::Dragged).willAlwaysCall(dragCb);
        rightTitleSide->getSignal(Event::LeftMousePressed).willAlwaysCall(activeCb);
        titlebar->getSignal(Event::Dragged).willAlwaysCall(dragCb);
        titlebar->getSignal(Event::LeftMousePressed).willAlwaysCall(activeCb);

        // Close button
        if (hasStyle(style, CloseButton)) {
            closeButton = Button::create("X");
            closeButton->setCharacterSize(16);
            closeButton->setExpandsWidth(true);
            closeButton->getSignal(Event::LeftClicked)
                .willAlwaysCall(std::bind(&Window::closed, this));
            rightTitleSide->pack(closeButton);
        }
    }

    elementArea->setExpandsWidth(true);
    elementArea->setExpandsHeight(true);
    elementArea->getSignal(Event::Dragged).willAlwaysCall(dragCb);
    assignAcquisition({position.x, position.y, 40, 20});

    Element* tmp[2] = {titlebar.get(), elementArea.get()};
    registerChildren(tmp);
}

void Window::handleDrag(const Event& action) {
    if (action.type() == Event::Dragged && moveable) {
        const sf::Vector2f dragAmount = action.dragStart() - action.mousePosition();
        const sf::Vector2f newPos =
            sf::Vector2f(getAcquisition().left, getAcquisition().top) - dragAmount;
        Element::setPosition(newPos);
    }
}

float Window::computeTitleHeight() const {
    rightTitleSide->setRequisition({0.f, 0.f});
    return titlebar ? std::max(titlebarHeight, titlebar->getRequisition().y) : 0.f;
}

float Window::computeTitleWidth() const { return titlebar ? titlebar->getRequisition().x : 0.f; }

void Window::onAcquisition() {
    const float h = computeTitleHeight();
    if (titlebar) {
        rightTitleSide->setRequisition({h, h});
        Packer::manuallyPackElement(
            titlebar,
            {getAcquisition().left, getAcquisition().top, getAcquisition().width, h},
            true);
    }
    Packer::manuallyPackElement(elementArea,
                                {getAcquisition().left,
                                 getAcquisition().top + h,
                                 getAcquisition().width,
                                 getAcquisition().height - h},
                                true);
    markClean();
}

sf::Vector2f Window::minimumRequisition() const {
    const sf::Vector2f childMin = elementArea->getRequisition();
    const float titleWidth      = computeTitleWidth();
    const float h               = childMin.y + computeTitleHeight();
    return {std::max(childMin.x, titleWidth), h > 10.f ? h : 10.f};
}

void Window::closed() { fireSignal(Event(Event::Closed)); }

rdr::Component* Window::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Window>(*this);
}

void Window::update(float dt) {
    Element::update(dt);

    if (dirty()) { assignAcquisition({getPosition(), getRequisition()}); }
    if (titlebar) { titlebar->update(dt); }
    elementArea->update(dt);
}

bool Window::shouldPack() const { return false; }

void Window::pack(const Element::Ptr& e) { elementArea->pack(e); }

void Window::pack(const Element::Ptr& e, bool fx, bool fy) { elementArea->pack(e, fx, fy); }

void Window::setTitlebarHeight(float h) {
    titlebarHeight = h;
    assignAcquisition({getPosition(), getRequisition()});
}

void Window::setPosition(const sf::Vector2f& pos) { Element::setPosition(pos); }

const Box::Ptr& Window::getElementArea() { return elementArea; }

const Label::Ptr& Window::getTitleLabel() { return title; }

const Box::Ptr& Window::getTitlebar() { return titlebar; }

const Button::Ptr& Window::getCloseButton() { return closeButton; }

bool Window::propagateEvent(const Event& event) {
    if (titlebar && titlebar->processEvent(event)) return true;
    if (elementArea->processEvent(event)) return true;
    return getAcquisition().contains(event.mousePosition());
}

float Window::getDepthBias() const { return -400.f; }

} // namespace gui
} // namespace bl
