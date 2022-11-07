#include <BLIB/Events/Dispatcher.hpp>

namespace bl
{
namespace event
{
void Dispatcher::syncListeners() {
    util::ReadWriteLock::WriteScopeGuard lock1(readWriteLock);
    std::lock_guard lock2(addLock);

    for (auto& p : toAdd) {
        auto lit = listeners.find(p.first);
        if (lit == listeners.end()) { lit = listeners.try_emplace(p.first).first; }
        auto sit = listenerDedup.find(p.first);
        if (sit == listenerDedup.end()) { sit = listenerDedup.try_emplace(p.first).first; }
        for (void* listener : p.second) {
            if (sit->second.find(listener) == sit->second.end()) {
                lit->second.emplace_back(listener);
                sit->second.emplace(listener);
            }
        }
    }
    toAdd.clear();

    for (auto& p : toRemove) {
        auto lit = listeners.find(p.first);
        if (lit == listeners.end()) { continue; }
        auto sit = listenerDedup.find(p.first);
        if (sit == listenerDedup.end()) { continue; }
        for (void* listener : p.second) {
            sit->second.erase(listener);
            for (unsigned int j = 0; j < lit->second.size(); ++j) {
                if (lit->second[j] == listener) {
                    lit->second.erase(lit->second.begin() + j);
                    --j;
                }
            }
        }
    }
    toRemove.clear();
}

} // namespace event
} // namespace bl
