#ifndef BLIB_GUI_SIGNAL_HPP
#define BLIB_GUI_SIGNAL_HPP

#include <BLIB/GUI/Action.hpp>
#include <BLIB/Util/Signal.hpp>

namespace bl
{
namespace gui
{
class Element;

using Signal = bl::Signal<const Action&, Element*>;

} // namespace gui
} // namespace bl

#endif
