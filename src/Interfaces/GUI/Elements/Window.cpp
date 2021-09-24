#include <BLIB/Interfaces/GUI/Elements/Window.hpp>

#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>

namespace bl
{
namespace gui
{
namespace
{
inline bool hasStyle(Window::Style style, Window::Style check) { return (style & check) == check; }
} // namespace

Window::Ptr Window::create(Packer::Ptr packer, const std::string& titleText, Style style,
                           const sf::Vector2i& position) {
    return Ptr(new Window(packer, titleText, style, position));
}

Window::Window(Packer::Ptr packer, const std::string& titleText, Style style,
               const sf::Vector2i& position)
: Element()
, moveable(hasStyle(style, Moveable))
, titlebarHeight(22)
, elementArea(Box::create(packer)) {
    using namespace std::placeholders;
    const Signal::Callback dragCb   = std::bind(&Window::handleDrag, this, _1);
    const Signal::Callback activeCb = std::bind(&Window::moveToTop, this);

    if (hasStyle(style, Titlebar)) {
        // Create titlebar containers
        titlebar       = Box::create(LinePacker::create(
            LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::LeftAlign));
        leftTitleSide  = Box::create(LinePacker::create(LinePacker::Horizontal));
        rightTitleSide = Box::create(LinePacker::create(
            LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::RightAlign));
        rightTitleSide->setRequisition(
            {static_cast<int>(titlebarHeight), static_cast<int>(titlebarHeight)});
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
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Window::onAcquisition, this));
    assignAcquisition({position.x, position.y, 40, 20});
}

void Window::handleDrag(const Event& action) {
    if (action.type() == Event::Dragged && moveable) {
        const sf::Vector2i dragAmount =
            static_cast<sf::Vector2i>(action.dragStart() - action.mousePosition());
        const sf::Vector2i newPos =
            sf::Vector2i(getAcquisition().left, getAcquisition().top) - dragAmount;
        Element::setPosition(newPos);

        fireSignal(
            Event(Event::Moved, static_cast<sf::Vector2f>(dragAmount), action.mousePosition()));
    }
}

int Window::computeTitleHeight() const {
    return titlebar ? std::max(static_cast<int>(titlebarHeight), titlebar->getRequisition().y) : 0;
}

int Window::computeTitleWidth() const { return titlebar ? titlebar->getRequisition().x : 0; }

void Window::onAcquisition() {
    const int h = computeTitleHeight();
    if (titlebar) {
        rightTitleSide->setRequisition({h, h});
        Packer::manuallyPackElement(
            titlebar, {getAcquisition().left, getAcquisition().top, getAcquisition().width, h});
    }
    Packer::manuallyPackElement(elementArea,
                                {getAcquisition().left,
                                 getAcquisition().top + h,
                                 getAcquisition().width,
                                 getAcquisition().height - h});
}

sf::Vector2i Window::minimumRequisition() const {
    const sf::Vector2i childMin = elementArea->getRequisition();
    const int titleWidth        = computeTitleWidth();
    const int h                 = childMin.y + computeTitleHeight();
    return {std::max(childMin.x, titleWidth), h > 10 ? h : 10};
}

void Window::closed() { fireSignal(Event(Event::Closed)); }

void Window::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      const Renderer& renderer) const {
    renderer.renderWindow(target, states, titlebar.get(), *this);
    if (titlebar) { titlebar->render(target, states, renderer); }
    elementArea->render(target, states, renderer);
}

void Window::update(float dt) {
    if (dirty()) {
        assignAcquisition(
            {sf::Vector2i(getAcquisition().left, getAcquisition().top), getRequisition()});
        markClean();
    }
    if (titlebar) { titlebar->update(dt); }
    elementArea->update(dt);
}

bool Window::shouldPack() const { return false; }

void Window::pack(Element::Ptr e) { elementArea->pack(e); }

void Window::pack(Element::Ptr e, bool fx, bool fy) { elementArea->pack(e, fx, fy); }

void Window::setTitlebarHeight(unsigned int h) {
    titlebarHeight = h;
    assignAcquisition(getAcquisition());
}

void Window::setPosition(const sf::Vector2i& pos) { Element::setPosition(pos); }

Box::Ptr Window::getElementArea() { return elementArea; }

Label::Ptr Window::getTitleLabel() { return title; }

Box::Ptr Window::getTitlebar() { return titlebar; }

Button::Ptr Window::getCloseButton() { return closeButton; }

bool Window::propagateEvent(const Event& event) {
    if (titlebar && titlebar->processEvent(event)) return true;
    if (elementArea->processEvent(event)) return true;
    return getAcquisition().contains(sf::Vector2i(event.mousePosition()));
}

} // namespace gui
} // namespace bl
