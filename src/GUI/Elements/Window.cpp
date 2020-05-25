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

Window::Window(Packer::Ptr packer, const std::string& titleText, Style style,
               const std::string& group, const std::string& id)
: Container(packer, group, id)
, hasBorder(!hasStyle(style, Borderless))
, moveable(hasStyle(style, Moveable)) {
    using namespace std::placeholders;

    if (hasStyle(style, Titlebar)) {
        /*titlebar =
            Container::create(LinePacker::create(LinePacker::Horizontal, LinePacker::Fill),
                              group + "-titlebar",
                              id + "-titlebar");
        leftTitleSide  = Container::create(LinePacker::create(LinePacker::Horizontal),
                                          group + "-leftTitlebar",
                                          id + "-leftTitlebar");
        rightTitleSide = Container::create(LinePacker::create(LinePacker::Horizontal,
                                                              LinePacker::Compact,
                                                              LinePacker::RightAlign),
                                           group + "-rightTitlebar",
                                           id + "-rightTitlebar");
        title          = Label::create(titleText, group + "-title", id + "-title");*/
        // TODO - set title text properties? REDO

        const Signal::Callback dragCb = std::bind(&Window::handleDrag, this, _1, _2);
        getSignal(Action::Dragged).willAlwaysCall(dragCb);
        title->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        leftTitleSide->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        rightTitleSide->getSignal(Action::Dragged).willAlwaysCall(dragCb);
        titlebar->getSignal(Action::Dragged).willAlwaysCall(dragCb);

        leftTitleSide->add(title);
        if (hasStyle(style, CloseButton)) {
            // TODO - add close button to right title side
        }
        titlebar->add(leftTitleSide);
        titlebar->add(rightTitleSide);
    }
}

void Window::update(float dt) {
    if (dragAmount.has_value() && moveable) {
        // TODO - implement offset/shifting/scaling in elements
        const sf::Vector2i newPos =
            sf::Vector2i(getAcquisition().left, getAcquisition().top) + dragAmount.value();
        assignAcquisition({newPos, minimumRequisition()});
        dragAmount.reset();
    }
    Container::update(dt);
}

void Window::handleDrag(const Action& action, Element*) {
    if (action.type == Action::Dragged)
        dragAmount = static_cast<sf::Vector2i>(action.data.dragStart - action.position);
}

void Window::doRender(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    if (titlebar) renderer->renderContainer(target, *titlebar);
    renderer->renderContainer(target, *this);
}

bool Window::borderless() const { return !hasBorder; }

bool Window::packable() const { return false; }

} // namespace gui
} // namespace bl