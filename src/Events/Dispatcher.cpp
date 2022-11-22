#include <BLIB/Events.hpp>

namespace bl
{
namespace event
{
namespace priv
{
struct ReserveOnStartupHelper {
    ReserveOnStartupHelper() { Dispatcher::dispatchers.reserve(128); }
} helper;
} // namespace priv

std::mutex Dispatcher::dispatcherLock;
std::vector<priv::SingleDispatcherBase*> Dispatcher::dispatchers;

void Dispatcher::syncListeners() {
    std::lock_guard lock(dispatcherLock);

    for (priv::SingleDispatcherBase* d : dispatchers) { d->syncListeners(); }
}

void Dispatcher::clearAllListeners() {
    std::lock_guard lock(dispatcherLock);

    for (priv::SingleDispatcherBase* d : dispatchers) { d->clear(); }
}

} // namespace event
} // namespace bl
