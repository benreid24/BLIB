#ifndef BLIB_SIGNALS_LISTENER_HPP
#define BLIB_SIGNALS_LISTENER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Signals/Channel.hpp>
#include <BLIB/Signals/Handler.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <tuple>

namespace bl
{
namespace sig
{
class Channel;

/**
 * @brief Base class for signal listeners. Create classes that derive from this to process signals
 *
 * @tparam ...TSignals The types of signals to process
 * @ingroup Signals
 */
template<typename... TSignals>
class Listener : public Handler<TSignals>... {
public:
    /**
     * @brief Initializes the listener
     */
    Listener();

    /**
     * @brief Unsubscribes from the connected channel and destroys the listener
     */
    virtual ~Listener();

    /**
     * @brief Subscribes to the given channel. Use subscribeDeferred if calling from a signal
     *        handler
     *
     * @param channel
     */
    void subscribe(Channel& channel);

    /**
     * @brief Defers subscription to the given channel. Use this if calling from a signal handler
     *
     * @param channel The channel to subscribe to. Will be synced at the start of the next frame
     */
    void subscribeDeferred(Channel& channel);

    /**
     * @brief Unsubscribes from the connected channel
     */
    void unsubscribe();

    /**
     * @brief Unsubscribes from the connected channel but defers the unsubscribe until the start of
     *        the next frame. Use this if calling from a signal handler
     */
    void unsubscribeDeferred();

    /**
     * @brief Returns whether the listener is currently subscribed to a channel
     */
    bool isSubscribed() const;

private:
    using CheckSignal = typename std::tuple_element<0, std::tuple<TSignals...>>::type;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TSignals>
Listener<TSignals...>::Listener() {}

template<typename... TSignals>
Listener<TSignals...>::~Listener() {
    unsubscribe();
}

template<typename... TSignals>
void Listener<TSignals...>::subscribe(Channel& channel) {
    if (isSubscribed()) {
        BL_LOG_WARN << "Listener already subscribed to a channel, replacing subscription";
    }

    (channel.getStream<TSignals>()->subscribe(static_cast<Handler<TSignals>*>(this)), ...);
};

template<typename... TSignals>
void Listener<TSignals...>::subscribeDeferred(Channel& channel) {
    if (isSubscribed()) {
        BL_LOG_WARN << "Listener already subscribed to a channel, replacing subscription";
    }

    (channel.getStream<TSignals>()->subscribeDeferred(static_cast<Handler<TSignals>*>(this)), ...);
};

template<typename... TSignals>
void Listener<TSignals...>::unsubscribe() {
    if (isSubscribed()) { (static_cast<Handler<TSignals>*>(this)->unsubscribe(), ...); }
}

template<typename... TSignals>
void Listener<TSignals...>::unsubscribeDeferred() {
    if (isSubscribed()) { (static_cast<Handler<TSignals>*>(this)->unsubscribeDeferred(), ...); }
}

template<typename... TSignals>
bool Listener<TSignals...>::isSubscribed() const {
    return Handler<CheckSignal>::subscribedTo != nullptr;
}

} // namespace sig
} // namespace bl

#endif
