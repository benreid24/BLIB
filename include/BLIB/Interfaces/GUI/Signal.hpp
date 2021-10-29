#ifndef BLIB_GUI_SIGNAL_HPP
#define BLIB_GUI_SIGNAL_HPP

#include <BLIB/Interfaces/GUI/Event.hpp>
#include <BLIB/Util/Signal.hpp>

namespace bl
{
namespace gui
{
class Element;

using Signal = bl::util::Signal<const Event&, Element*>;

} // namespace gui
} // namespace bl

#endif
