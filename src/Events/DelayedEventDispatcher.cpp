#include <BLIB/Events/DelayedEventDispatcher.hpp>

#include <chrono>

namespace bl
{
DelayedEventDispatcher::DelayedEventDispatcher(MultiEventDispatcher& d, bool mt)
: underlying(d)
, running(mt)
, runner(&DelayedEventDispatcher::background, this) {}

DelayedEventDispatcher::~DelayedEventDispatcher() {
    running = false;
    cvar.notify_all();
    runner.join();
    drain();
}

void DelayedEventDispatcher::background() {
    while (running) {
        {
            std::unique_lock lock(mutex);
            cvar.wait(lock);
            drainAll();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}

void DelayedEventDispatcher::drain() {
    std::unique_lock lock(mutex);
    drainAll();
}

void DelayedEventDispatcher::drainAll() {
    for (container::Any<32>& d : events) { static_cast<Dispatch*>(d.raw())->dispatch(underlying); }
    events.clear();
}

} // namespace bl
