#include <BLIB/GUI/Signal.hpp>

namespace bl
{
namespace gui
{
void Signal::willSet(bool& var, bool val) { setActions.push_back(std::make_pair(&var, val)); }

void Signal::willCall(Callback cb) { userCallbacks.push_back(cb); }

void Signal::clear() {
    setActions.clear();
    userCallbacks.clear();
}

void Signal::operator()(const Action& action, Element* element) {
    for (const auto& set : setActions) { *set.first = set.second; }
    for (const auto& cb : userCallbacks) { cb(action, element); }
    for (const auto& cb : internalCallbacks) { cb(action, element); }
}

} // namespace gui
} // namespace bl