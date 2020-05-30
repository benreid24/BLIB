#include <BLIB/GUI/Elements/Window.hpp>

#include <BLIB/GUI/Packers/LinePacker.hpp>

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
    Ptr window(new Window(packer, titleText, style, position, group, id));
    window->addTitlebar();
    return window;
}

Window::Window(Packer::Ptr packer, const std::string& titleText, Style style,
               const sf::Vector2i& position, const std::string& group, const std::string& id)
: Container(packer, group, id)
, moveable(hasStyle(style, Moveable))
, titlebarHeight(22) {
    using namespace std::placeholders;

    // we will pack children into a smaller acquisition under the titlebar
    autopack() = false;

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
        const Signal::Callback dragCb   = std::bind(&Window::handleDrag, this, _1);
        const Signal::Callback activeCb = std::bind(&Window::titleActive, this);
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
            closeButton = Button::create("X", group + "-close", id + "-close");
            closeButton->setCharacterSize(16);
            closeButton->setExpandsWidth(true);
            closeButton->getSignal(Action::LeftClicked)
                .willAlwaysCall(std::bind(&Window::closed, this));
            rightTitleSide->add(closeButton);
        }
    }

    getSignal(Action::AcquisitionChanged)
        .willAlwaysCall(std::bind(&Window::onAcquisition, this));
    assignAcquisition({position.x, position.y, 40, 20});
}

void Window::addTitlebar() {
    if (titlebar) {
        add(titlebar);
        markForManualPack(titlebar);
        markForManualRender(titlebar);
    }
}

void Window::handleDrag(const Action& action) {
    if (action.type == Action::Dragged && moveable) {
        const sf::Vector2i dragAmount =
            static_cast<sf::Vector2i>(action.data.dragStart - action.position);
        const sf::Vector2i newPos =
            sf::Vector2i(getAcquisition().left, getAcquisition().top) - dragAmount;
        Element::setPosition(newPos);

        fireSignal(
            Action(Action::Moved, static_cast<sf::Vector2f>(dragAmount), action.position));
    }
}

void Window::onAcquisition() {
    const int h = static_cast<int>(titlebarHeight);
    if (titlebar) {
        rightTitleSide->setRequisition({h, h});
        Packer::manuallyPackElement(titlebar, {0, 0, getAcquisition().width, h});
    }
    packChildren({0, h, getAcquisition().width, getAcquisition().height - h});
}

void Window::makeClean() {
    assignAcquisition(
        {getAcquisition().left, getAcquisition().top, getRequisition().x, getRequisition().y});
}

sf::Vector2i Window::minimumRequisition() const {
    const sf::Vector2i childMin = Container::minimumRequisition();
    const int h                 = childMin.y + static_cast<int>(titlebarHeight);
    return {childMin.x, h > 10 ? h : 10};
}

void Window::closed() { fireSignal(Action(Action::Closed)); }

void Window::titleActive() { moveToTop(); }

void Window::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      Renderer::Ptr renderer) const {
    renderer->renderWindow(target, states, titlebar.get(), *this);
    renderChildren(target, states, renderer);
    if (titlebar)
        manuallyRenderChild(titlebar, target, states, renderer); // renders children only
}

bool Window::packable() const { return false; }

void Window::setTitlebarHeight(unsigned int h) {
    titlebarHeight = h;
    assignAcquisition(getAcquisition());
}

void Window::setPosition(const sf::Vector2i& pos) { Element::setPosition(pos); }

Label::Ptr Window::getTitleLabel() { return title; }

Container::Ptr Window::getTitlebar() { return titlebar; }

Button::Ptr Window::getCloseButton() { return closeButton; }

} // namespace gui
} // namespace bl