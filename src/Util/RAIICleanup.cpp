#include <BLIB/Util/RAIICleanup.hpp>

namespace bl
{
namespace util
{
RAIICleanup::RAIICleanup(unsigned int hint)
: armed(true) {
    tasks.reserve(hint);
}

RAIICleanup::~RAIICleanup() {
    if (armed) {
        for (auto rit = tasks.rbegin(); rit != tasks.rend(); ++rit) { (*rit)(); }
    }
}

void RAIICleanup::add(std::function<void()> cleanupFunc) {
    if (armed) { tasks.push_back(std::move(cleanupFunc)); }
}

void RAIICleanup::disarm() { armed = false; }

} // namespace util
} // namespace bl
