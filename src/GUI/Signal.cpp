#include <BLIB/GUI/Signal.hpp>

namespace bl
{
namespace gui
{
void Signal::willSet(bool& var, bool val) { setAction = std::make_pair(&var, val); }

void Signal::willCall(Callback cb) { callbackAction = cb; }

void Signal::clear() {
    setAction.reset();
    callbackAction.reset();
}

void Signal::operator()(const Action& action, Element* element) {
    if (setAction.has_value()) { *setAction.value().first = setAction.value().second; }
    if (callbackAction.has_value()) { callbackAction.value()(action, element); }
}

} // namespace gui
} // namespace bl