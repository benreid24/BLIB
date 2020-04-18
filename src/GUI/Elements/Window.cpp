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
, moveable(hasStyle(style, Moveable)) {
    using namespace std::placeholders;
    const Signal::Callback dragCb = std::bind(&Window::handleDrag, this, _1, _2);

    if (hasStyle(style, Titlebar)) {
        titlebar =
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
        title          = Label::create(titleText, group + "-title", id + "-title");
        // TODO - set title text properties?

        title->getSignal(Action::Dragged).willCall(dragCb);
        leftTitleSide->getSignal(Action::Dragged).willCall(dragCb);
        rightTitleSide->getSignal(Action::Dragged).willCall(dragCb);
        titlebar->getSignal(Action::Dragged).willCall(dragCb);

        leftTitleSide->add(title);
        if (hasStyle(style, CloseButton)) {
            // TODO - add close button to right title side
        }
        titlebar->add(leftTitleSide);
        titlebar->add(rightTitleSide);
    }
}

} // namespace gui
} // namespace bl