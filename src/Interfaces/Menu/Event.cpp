#include <BLIB/Interfaces/Menu/Event.hpp>

namespace bl
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
} // namespace bl
