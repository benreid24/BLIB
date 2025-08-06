#ifndef BLIB_SIGNALS_STREAM_HPP
#define BLIB_SIGNALS_STREAM_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Signals/Handler.hpp>
#include <BLIB/Signals/Priv/StreamBase.hpp>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace bl
{
/// Contains all the classes and functionality for signals
namespace sig
{
template<typename T>
class Stream : public priv::StreamBase {
public:
    Stream(std::size_t capacityHint = 16);

    virtual ~Stream();

    void subscribe(Handler<T>* handler);

    void subscribeDeferred(Handler<T>* handler);

    void unsubscribe(Handler<T>* handler);

    void unsubscribeDeferred(Handler<T>* handler);

    void signal(const T& signal);

    void signalSynchronized(const T& signal);

    virtual void syncDeferred() override;

private:
    ctr::FastEraseVector<Handler<T>*> listeners;
    std::vector<Handler<T>*> toAdd;
    std::vector<Handler<T>*> toRemove;
    std::unordered_set<Handler<T>*> allHandlers;
    std::mutex mutex;
    std::mutex deferMutex;
    std::atomic_bool needDeferSync;

    void doAdd(Handler<T>* handler);
    void doRemove(Handler<T>* handler);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Stream<T>::Stream(std::size_t capacityHint)
: needDeferSync(false) {
    listeners.reserve(capacityHint);
}

template<typename T>
Stream<T>::~Stream() {
    for (Handler<T>* handler : allHandlers) { handler->subscribedTo = nullptr; }
}

template<typename T>
void Stream<T>::subscribe(Handler<T>* handler) {
    if (handler->subscribedTo && handler->subscribedTo != this) { handler->unsubscribeDeferred(); }

    std::unique_lock lock(mutex);
    doAdd(handler);
}

template<typename T>
void Stream<T>::subscribeDeferred(Handler<T>* handler) {
    if (handler->subscribedTo && handler->subscribedTo != this) { handler->unsubscribeDeferred(); }

    std::unique_lock lock(deferMutex);
    toAdd.emplace_back(handler);
    needDeferSync = true;
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
    needDeferSync = true;
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
    if (!needDeferSync) return;

    std::unique_lock mainLock(mutex);
    std::unique_lock deferLock(deferMutex);

    needDeferSync = false;
    for (Handler<T>* handler : toAdd) { doAdd(handler); }
    for (Handler<T>* handler : toRemove) { doRemove(handler); }
    toAdd.clear();
    toRemove.clear();
}

template<typename T>
void Stream<T>::doAdd(Handler<T>* handler) {
    handler->subscribedTo = this;
    if (allHandlers.emplace(handler).second) { listeners.emplace_back(handler); }
}

template<typename T>
void Stream<T>::doRemove(Handler<T>* handler) {
    handler->subscribedTo = nullptr;
    if (allHandlers.erase(handler) > 0) {
        for (auto it = listeners.begin(); it != listeners.end(); ++it) {
            if (*it == handler) {
                listeners.erase(it);
                return;
            }
        }
    }
}

template<typename T>
Handler<T>::~Handler() {
    unsubscribe();
}

template<typename T>
void Handler<T>::unsubscribe() {
    if (subscribedTo) {
        subscribedTo->unsubscribe(this);
        subscribedTo = nullptr;
    }
}

template<typename T>
void Handler<T>::unsubscribeDeferred() {
    if (subscribedTo) {
        subscribedTo->unsubscribeDeferred(this);
        subscribedTo = nullptr;
    }
}

} // namespace sig
} // namespace bl

#endif
