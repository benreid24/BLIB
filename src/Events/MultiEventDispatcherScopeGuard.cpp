#include <BLIB/Events/MultiEventDispatcherScopeGuard.hpp>

namespace bl
{
MultiEventDispatcherScopeGuard::MultiEventDispatcherScopeGuard(MultiEventDispatcher& dispatcher)
: dispatcher(dispatcher) {}

MultiEventDispatcherScopeGuard::~MultiEventDispatcherScopeGuard() {
    for (const auto tpair : listeners) {
        for (void* l : tpair.second) { dispatcher.remove(tpair.first, l); }
    }
}

void MultiEventDispatcherScopeGuard::remove(const std::type_index& t, void* val) {
    auto lit = listeners.find(t);
    if (lit != listeners.end()) {
        for (unsigned int j = 0; j < lit->second.size(); ++j) {
            if (lit->second[j] == val) {
                lit->second.erase(lit->second.begin() + j);
                --j;
            }
        }
    }
}

} // namespace bl
