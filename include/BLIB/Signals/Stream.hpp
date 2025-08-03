#ifndef BLIB_SIGNALS_STREAM_HPP
#define BLIB_SIGNALS_STREAM_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Signals/Handler.hpp>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace bl
{
/// Contains all the classes and functionality for signals
namespace sig
{
template<typename T>
class Stream {
public:
    Stream(std::size_t capacityHint = 16);

    void subscribe(Handler<T>* handler);

    void subscribeDeferred(Handler<T>* handler);

    void unsubscribe(Handler<T>* handler);

    void unsubscribeDeferred(Handler<T>* handler);

    void signal(const T& signal);

    void signalSynchronized(const T& signal);

    void syncDeferred();

private:
    ctr::FastEraseVector<Handler<T>*> listeners;
    std::vector<Handler<T>*> toAdd;
    std::vector<Handler<T>*> toRemove;
    std::unordered_set<Handler<T>*> allHandlers;
    std::mutex mutex;
    std::mutex deferMutex;

    bool doAdd(Handler<T>* handler);
    void doRemove(Handler<T>* handler);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Stream<T>::Stream(std::size_t capacityHint) {
    listeners.reserve(capacityHint);
}

template<typename T>
void Stream<T>::subscribe(Handler<T>* handler) {
    std::unique_lock lock(mutex);
    doAdd(handler);
}

template<typename T>
void Stream<T>::subscribeDeferred(Handler<T>* handler) {
    std::unique_lock lock(deferMutex);
    toAdd.emplace_back(handler);
}

template<typename T>
void Stream<T>::unsubscribe(Handler<T>* handler) {
    std::unique_lock lock(mutex);
    doRemove(handler);
}

template<typename T>
void Stream<T>::unsubscribeDeferred(Handler<T>* handler) {
    std::unique_lock lock(deferMutex);
    toRemove.emplace_back(handler);
}

template<typename T>
void Stream<T>::signal(const T& signal) {
    for (Handler<T>* handler : listeners) { handler->process(signal); }
}

template<typename T>
void Stream<T>::signalSynchronized(const T& signal) {
    std::unique_lock lock(mutex);
    for (Handler<T>* handler : listeners) { handler->process(signal); }
}

template<typename T>
void Stream<T>::syncDeferred() {
    std::unique_lock mainLock(mutex);
    std::unique_lock deferLock(deferMutex);

    for (Handler<T>* handler : toAdd) { doAdd(handler); }
    for (Handler<T>* handler : toRemove) { doRemove(handler); }
    toAdd.clear();
    toRemove.clear();
}

template<typename T>
bool Stream<T>::doAdd(Handler<T>* handler, Handler<T>* replacement) {
    if (allHandlers.emplace(handler).second) { listeners.emplace_back(handler); }
}

template<typename T>
void Stream<T>::doRemove(Handler<T>* handler) {
    if (allHandlers.erase(handler) > 0) {
        for (auto it = listeners.begin(); it != listeners.end(); ++it) {
            if (*it == handler) {
                listeners.erase(it);
                return;
            }
        }
    }
}

} // namespace sig
} // namespace bl

#endif
