#include <BENG/Menu/Event.hpp>

namespace bg
{
namespace menu
{
Event::Event(const MoveEvent& moveEvent)
: type(SelectorMove)
, moveEvent(moveEvent) {}

Event::Event(const ActivateEvent& activateEvent)
: type(Activate)
, activateEvent(activateEvent) {}

Event::Event(const LocationEvent& locationEvent)
: type(SelectorLocation)
, locationEvent(locationEvent) {}

} // namespace menu
} // namespace bg
