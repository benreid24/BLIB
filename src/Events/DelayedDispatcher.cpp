#include <BLIB/Events/DelayedDispatcher.hpp>

#include <chrono>

namespace bl
{
namespace event
{
DelayedDispatcher::DelayedDispatcher(Dispatcher& d, bool mt)
: underlying(d)
, running(mt)
, runner(&DelayedDispatcher::background, this) {}

DelayedDispatcher::~DelayedDispatcher() {
    running = false;
    cvar.notify_all();
    runner.join();
    drain();
}

void DelayedDispatcher::background() {
    while (running) {
        {
            std::unique_lock lock(mutex);
            cvar.wait(lock);
            drainAll();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}

void DelayedDispatcher::drain() {
    std::unique_lock lock(mutex);
    drainAll();
}

void DelayedDispatcher::drainAll() {
    for (container::Any<32>& d : events) { static_cast<Dispatch*>(d.raw())->dispatch(underlying); }
    events.clear();
}

} // namespace event
} // namespace bl
