#ifndef BLIB_EVENTS_SINGLEDISPATCHER_HPP
#define BLIB_EVENTS_SINGLEDISPATCHER_HPP

#include <BLIB/Events/Listener.hpp>
#include <BLIB/Util/ReadWriteLock.hpp>
#include <unordered_set>
#include <vector>

namespace bl
{
namespace event
{
class Dispatcher;

/// @brief Private namespace. Contained classes are internal implementation details
namespace priv
{
class SingleDispatcherBase {
    virtual void syncListeners() = 0;
    virtual void clear()         = 0;

    friend class ::bl::event::Dispatcher;
};

template<typename T>
class SingleDispatcher : private SingleDispatcherBase {
    static SingleDispatcher& get(std::vector<SingleDispatcherBase*>& dlist, std::mutex& dlock) {
        static SingleDispatcher d(dlist, dlock);
        return d;
    }

    SingleDispatcher(std::vector<SingleDispatcherBase*>& dlist, std::mutex& dlock) {
        listeners.reserve(256);
        toAdd.reserve(32);
        toRemove.reserve(32);

        std::lock_guard lock(dlock);
        dlist.push_back(this);
    }

    void dispatch(const T& event) {
        util::ReadWriteLock::ReadScopeGuard lock(busLock);
        for (ListenerBase<T>* l : listeners) { l->observe(event); }
    }

    void addListener(ListenerBase<T>* listener, bool defer) {
        if (defer) {
            std::lock_guard lock(deferLock);
            toAdd.emplace_back(listener);
        }
        else {
            util::ReadWriteLock::WriteScopeGuard lock(busLock);
            doAdd(listener);
        }
    }

    void removeListener(ListenerBase<T>* listener, bool defer) {
        if (defer) {
            std::lock_guard lock(deferLock);
            toRemove.emplace_back(listener);
        }
        else {
            util::ReadWriteLock::WriteScopeGuard lock(busLock);
            doRemove(listener);
        }
    }

    virtual void syncListeners() override {
        util::ReadWriteLock::WriteScopeGuard lock(busLock);
        std::lock_guard lock2(deferLock);

        for (ListenerBase<T>* listener : toAdd) { doAdd(listener); }
        for (ListenerBase<T>* listener : toRemove) { doRemove(listener); }
        toAdd.clear();
        toRemove.clear();
    }

    virtual void clear() override {
        util::ReadWriteLock::WriteScopeGuard lock(busLock);
        std::lock_guard lock2(deferLock);
        listeners.clear();
        toAdd.clear();
        toRemove.clear();
    }

    void doAdd(ListenerBase<T>* listener) {
        if (dedup.emplace(listener).second) { listeners.emplace_back(listener); }
    }

    void doRemove(ListenerBase<T>* listener) {
        if (dedup.erase(listener) > 0) {
            for (auto it = listeners.begin(); it != listeners.end(); ++it) {
                if (*it == listener) {
                    listeners.erase(it);
                    break;
                }
            }
        }
    }

    util::ReadWriteLock busLock;
    std::vector<ListenerBase<T>*> listeners;
    std::unordered_set<ListenerBase<T>*> dedup;

    std::mutex deferLock;
    std::vector<ListenerBase<T>*> toAdd;
    std::vector<ListenerBase<T>*> toRemove;

    friend class ::bl::event::Dispatcher;
    template<typename U>
    friend class SubscriberBase;
    template<typename... U>
    friend class SubscriberHelper;
    template<typename U>
    friend class UnSubscriberBase;
    template<typename... U>
    friend class UnSubscriberHelper;
};

} // namespace priv
} // namespace event
} // namespace bl

#endif
