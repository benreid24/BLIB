#ifndef BLIB_SIGNALS_STREAM_HPP
#define BLIB_SIGNALS_STREAM_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Logging.hpp>
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
/**
 * @brief Stream of signals of a single type. Owned by Channel. Use Channel to send and subscribe to
 *        signals
 *
 * @tparam T The type of signal to stream
 * @ingroup Signals
 */
template<typename T>
class Stream : public priv::StreamBase {
public:
    /**
     * @brief Creates a new stream
     *
     * @param capacityHint The expected number of listeners
     */
    Stream(std::size_t capacityHint = 16);

    /**
     * @brief Destroys the stream and unsubscribes all listeners and emitters
     */
    virtual ~Stream();

    /**
     * @brief Subscribes the given handler to the stream
     *
     * @param handler The handler to subscribe
     */
    void subscribe(Handler<T>* handler);

    /**
     * @brief Defers subscription of the given handler to the stream
     *
     * @param handler The handler to subscribe
     */
    void subscribeDeferred(Handler<T>* handler);

    /**
     * @brief Unsubscribes the given handler to the stream
     *
     * @param handler The handler to unsubscribe
     */
    void unsubscribe(Handler<T>* handler);

    /**
     * @brief Defers unsubscription the given handler to the stream
     *
     * @param handler The handler to unsubscribe
     */
    void unsubscribeDeferred(Handler<T>* handler);

    /**
     * @brief Sends the given signal to all subscribed handlers
     *
     * @param signal The signal to send
     */
    void signal(const T& signal);

    /**
     * @brief Sends the given signal to all subscribed handlers. Use this if other threads may be
     *        calling subscribe or unsubscribe
     *
     * @param signal The signal to send
     */
    void signalSynchronized(const T& signal);

    /**
     * @brief Performs any deferred subscriptions or unsubscriptions. Called by the engine
     */
    virtual void syncDeferred() override;

private:
    ctr::FastEraseVector<Handler<T>*> listeners;
    std::vector<Handler<T>*> toAdd;
    std::vector<Handler<T>*> toRemove;
    std::unordered_set<Handler<T>*> allHandlers;
    std::mutex mutex;
    std::mutex deferMutex;
    std::atomic_bool needDeferSync;
    std::atomic_bool emitting;

    void doAdd(Handler<T>* handler);
    void doRemove(Handler<T>* handler);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Stream<T>::Stream(std::size_t capacityHint)
: needDeferSync(false)
, emitting(false) {
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
    if (!emitting) { doAdd(handler); }
    else {
        BL_LOG_WARN << "Use subscribeDeferred when subscribing from a signal handler. Deferring "
                       "subscription";
        std::unique_lock deferLock(deferMutex);
        toAdd.emplace_back(handler);
        needDeferSync = true;
    }
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

    if (!emitting) { doRemove(handler); }
    else {
        BL_LOG_WARN
            << "Use unsubscribeDeferred when unsubscribing from a signal handler. Deferring "
               "unsubscribe";
        std::unique_lock deferLock(deferMutex);
        toRemove.emplace_back(handler);
        needDeferSync = true;
    }
}

template<typename T>
void Stream<T>::unsubscribeDeferred(Handler<T>* handler) {
    std::unique_lock lock(deferMutex);
    toRemove.emplace_back(handler);
    needDeferSync = true;
}

template<typename T>
void Stream<T>::signal(const T& signal) {
    emitting = true;
    for (Handler<T>* handler : listeners) { handler->process(signal); }
    emitting = false;
}

template<typename T>
void Stream<T>::signalSynchronized(const T& signal) {
    std::unique_lock lock(mutex);
    emitting = true;
    for (Handler<T>* handler : listeners) { handler->process(signal); }
    emitting = false;
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
