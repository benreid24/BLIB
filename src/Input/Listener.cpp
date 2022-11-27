#include <BLIB/Input/Listener.hpp>

#include <BLIB/Input/Actor.hpp>

namespace bl
{
namespace input
{
Listener::Listener()
: owner(nullptr) {}

Listener::Listener(const Listener& l)
: owner(l.owner) {
    Listener& c = const_cast<Listener&>(l);
    if (owner) { owner->replaceListener(c, *this); }
}

Listener::Listener(Listener&& l)
: owner(l.owner) {
    if (owner) { owner->replaceListener(l, *this); }
    l.owner = nullptr;
}

Listener::~Listener() {
    if (owner) { owner->removeListener(*this); }
}

} // namespace input
} // namespace bl
