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
    if (owner) { owner->replaceListener(l, *this); }
    const_cast<Listener&>(l).owner = nullptr;
}

Listener::Listener(Listener&& l)
: owner(l.owner) {
    if (owner) { owner->replaceListener(l, *this); }
    l.owner = nullptr;
}

} // namespace input
} // namespace bl
