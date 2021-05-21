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
                           const sf::Vector2i& position, const std::string& group,
                           const std::string& id) {
    Ptr window(new Window(packer, titleText, style, position, group, id));
    window->addChildren();
    return window;
}

Window::Window(Packer::Ptr packer, const std::string& titleText, Style style,
               const sf::Vector2i& position, const std::string& group, const std::string& id)
: Container(group, id)
, elementArea(Box::create(packer, group, id + "-elementArea"))
, moveable(hasStyle(style, Moveable))
, titlebarHeight(22) {
    using namespace std::placeholders;
    const Signal::Callback dragCb   = std::bind(&Window::handleDrag, this, _1);
    const Signal::Callback activeCb = std::bind(&Window::titleActive, this);

    if (hasStyle(style, Titlebar)) {
        // Create titlebar containers
        titlebar =
            Box::create(LinePacker::create(
                            LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::LeftAlign),
                        group,
                        id + "-titlebar");
        leftTitleSide =
            Box::create(LinePacker::create(LinePacker::Horizontal), group, id + "-leftTitlebar");
        rightTitleSide =
            Box::create(LinePacker::create(
                            LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::RightAlign),
                        group,
                        id + "-rightTitlebar");
        rightTitleSide->setRequisition(
            {static_cast<int>(titlebarHeight), static_cast<int>(titlebarHeight)});
        titlebar->pack(leftTitleSide, true, true);
        titlebar->pack(rightTitleSide);
        titlebar->setExpandsWidth(true);
        titlebar->setExpandsHeight(true);

        // Title text
        title = Label::create(titleText, group, id + "-title");
        title->setCharacterSize(18);
        title->setHorizontalAlignment(RenderSettings::Left);
        leftTitleSide->pack(title, true, true);

        // Setup drag action
        getSignal(Action::Dragged).willAlwaysCall(dragCb);
        title->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        title->getSignal(Action::Pressed).willAlwaysCall(activeCb);
        leftTitleSide->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        leftTitleSide->getSignal(Action::Pressed).willAlwaysCall(activeCb);
        rightTitleSide->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        rightTitleSide->getSignal(Action::Pressed).willAlwaysCall(activeCb);
        titlebar->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        titlebar->getSignal(Action::Pressed).willAlwaysCall(activeCb);

        // Close button
        if (hasStyle(style, CloseButton)) {
            closeButton = Button::create("X", group, id + "-close");
            closeButton->setCharacterSize(16);
            closeButton->setExpandsWidth(true);
            closeButton->getSignal(Action::LeftClicked)
                .willAlwaysCall(std::bind(&Window::closed, this));
            rightTitleSide->pack(closeButton);
        }
    }

    elementArea->setExpandsWidth(true);
    elementArea->setExpandsHeight(true);
    elementArea->getSignal(Action::Dragged).willAlwaysCall(dragCb);
    assignAcquisition({position.x, position.y, 40, 20});
}

void Window::handleDrag(const Action& action) {
    if (action.type == Action::Dragged && moveable) {
        const sf::Vector2i dragAmount =
            static_cast<sf::Vector2i>(action.data.dragStart - action.position);
        const sf::Vector2i newPos =
            sf::Vector2i(getAcquisition().left, getAcquisition().top) - dragAmount;
        Element::setPosition(newPos);

        fireSignal(Action(Action::Moved, static_cast<sf::Vector2f>(dragAmount), action.position));
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
        Packer::manuallyPackElement(titlebar, {0, 0, getAcquisition().width, h});
    }
    Packer::manuallyPackElement(elementArea,
                                {0, h, getAcquisition().width, getAcquisition().height - h});
}

sf::Vector2i Window::minimumRequisition() const {
    const sf::Vector2i childMin = elementArea->getRequisition();
    const int titleWidth        = computeTitleWidth();
    const int h                 = childMin.y + computeTitleHeight();
    return {std::max(childMin.x, titleWidth), h > 10 ? h : 10};
}

void Window::closed() { fireSignal(Action(Action::Closed)); }

void Window::titleActive() { moveToTop(); }

void Window::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      const Renderer& renderer) const {
    renderer.renderWindow(target, states, titlebar.get(), *this);
    renderChildren(target, states, renderer);
}

void Window::update(float dt) {
    if (dirty())
        assignAcquisition(
            {sf::Vector2i(getAcquisition().left, getAcquisition().top), getRequisition()});
    Container::update(dt);
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

void Window::addChildren() {
    if (titlebar) add(titlebar);
    add(elementArea);
}

} // namespace gui
} // namespace bl