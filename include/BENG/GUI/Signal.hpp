#ifndef BLIB_GUI_SIGNAL_HPP
#define BLIB_GUI_SIGNAL_HPP

#include <BENG/GUI/Action.hpp>
#include <BENG/Util/Signal.hpp>

namespace bg
{
namespace gui
{
class Element;

using Signal = bg::Signal<const Action&, Element*>;

} // namespace gui
} // namespace bg

#endif
