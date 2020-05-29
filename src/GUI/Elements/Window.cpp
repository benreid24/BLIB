#include <BLIB/GUI/Elements/Window.hpp>

#include <BLIB/GUI/Packers/LinePacker.hpp>

#include <iostream>

namespace bl
{
namespace gui
{
namespace
{
inline bool hasStyle(Window::Style style, Window::Style check) { return style & check != 0; }
} // namespace

Window::Ptr Window::create(Packer::Ptr packer, const std::string& titleText, Style style,
                           const sf::Vector2i& position, const std::string& group,
                           const std::string& id) {
    return Ptr(new Window(packer, titleText, style, position, group, id));
}

Window::Window(Packer::Ptr packer, const std::string& titleText, Style style,
               const sf::Vector2i& position, const std::string& group, const std::string& id)
: Container(packer, group, id)
, moveable(hasStyle(style, Moveable))
, titlebarHeight(22) {
    using namespace std::placeholders;

    if (hasStyle(style, Titlebar)) {
        // Create titlebar containers
        titlebar = Container::create(
            LinePacker::create(
                LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::LeftAlign),
            group + "-titlebar",
            id + "-titlebar");
        leftTitleSide  = Container::create(LinePacker::create(LinePacker::Horizontal),
                                          group + "-leftTitlebar",
                                          id + "-leftTitlebar");
        rightTitleSide = Container::create(
            LinePacker::create(
                LinePacker::Horizontal, 2, LinePacker::Compact, LinePacker::RightAlign),
            group + "-rightTitlebar",
            id + "-rightTitlebar");
        rightTitleSide->setRequisition(
            {static_cast<int>(titlebarHeight), static_cast<int>(titlebarHeight)});
        titlebar->add(leftTitleSide, true, true);
        titlebar->add(rightTitleSide);
        titlebar->setExpandsWidth(true);
        titlebar->setExpandsHeight(true);

        // Title text
        title = Label::create(titleText, group + "-title", id + "-title");
        title->setCharacterSize(18);
        leftTitleSide->add(title, true, true);

        // Setup drag action
        const Signal::Callback dragCb = std::bind(&Window::handleDrag, this, _1, _2);
        getSignal(Action::Dragged).willAlwaysCall(dragCb);
        title->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        leftTitleSide->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        rightTitleSide->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        titlebar->getSignal(Action::Dragged).willAlwaysCall(dragCb);

        // Close button
        if (hasStyle(style, CloseButton)) {
            closeButton = Button::create("X", group + "-close", id + "-close");
            closeButton->setCharacterSize(16);
            closeButton->setExpandsWidth(true);
            closeButton->getSignal(Action::LeftClicked)
                .willAlwaysCall(std::bind(&Window::closed, this, _1, _2));
            rightTitleSide->add(closeButton);
        }
    }

    getSignal(Action::AcquisitionChanged)
        .willAlwaysCall(std::bind(&Window::onAcquisition, this, _1, _2));
    assignAcquisition({position.x, position.y, 40, 20});
}

void Window::handleDrag(const Action& action, Element*) {
    if (action.type == Action::Dragged && moveable) {
        const sf::Vector2i dragAmount =
            static_cast<sf::Vector2i>(action.data.dragStart - action.position);
        const sf::Vector2i newPos =
            sf::Vector2i(getAcquisition().left, getAcquisition().top) - dragAmount;
        setPosition(newPos);

        fireSignal(
            Action(Action::Moved, static_cast<sf::Vector2f>(dragAmount), action.position));
    }
}

void Window::onAcquisition(const Action&, Element*) {
    const int h = static_cast<int>(titlebarHeight);
    if (titlebar) {
        rightTitleSide->setRequisition({h, h});
        Packer::manuallyPackElement(
            titlebar,
            {getAcquisition().left, getAcquisition().top - h, getAcquisition().width, h});
    }
}

void Window::closed(const Action&, Element*) { fireSignal(Action(Action::Closed)); }

void Window::update(float dt) {
    if (dirty()) {
        assignAcquisition({getAcquisition().left,
                           getAcquisition().top,
                           getRequisition().x,
                           getRequisition().y});
    }
    Container::update(dt);
}

void Window::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      Renderer::Ptr renderer) const {
    renderer->renderWindow(target, states, titlebar.get(), *this);
    if (titlebar) titlebar->render(target, states, renderer); // renders children only
    renderChildren(target, states, renderer);
}

bool Window::packable() const { return false; }

bool Window::handleRawEvent(const RawEvent& event) {
    if (titlebar && titlebar->handleEvent(event)) return true;
    return Container::handleRawEvent(event);
}

void Window::setTitlebarHeight(unsigned int h) {
    titlebarHeight = h;
    assignAcquisition(getAcquisition());
}

void Window::setPosition(const sf::Vector2i& pos) {
    assignAcquisition({pos, getRequisition()});
}

Label::Ptr Window::getTitleLabel() { return title; }

Container::Ptr Window::getTitlebar() { return titlebar; }

Button::Ptr Window::getCloseButton() { return closeButton; }

} // namespace gui
} // namespace bl