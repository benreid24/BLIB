#ifndef BLIB_SIGNALS_ROUTER_HPP
#define BLIB_SIGNALS_ROUTER_HPP

#include <BLIB/Signals/Handler.hpp>
#include <functional>
#include <tuple>

namespace bl
{
namespace sig
{
template<typename... TSignals>
class Router;

namespace priv
{
template<typename T>
class Route : public Handler<T> {
public:
    virtual ~Route() = default;

private:
    std::function<void(const T&)> callback;

    virtual void process(const T& signal) override {
        if (callback) { callback(signal); }
    }

    template<typename... TSignals>
    friend class bl::sig::Router;
};
} // namespace priv

/**
 * @brief Utility to route signals to callbacks. Is instantiated and callbacks are registered for
 *        specific signal types. Allows a single class to subscribe to multiple channels via the use
 *        of multiple Routers
 *
 * @tparam ...TSignals The types of signals to route
 * @ingroup Signals
 */
template<typename... TSignals>
class Router : public priv::Route<TSignals>... {
public:
    /**
     * @brief Creates the router
     */
    Router();

    /**
     * @brief Destroys the router and unsubscribes
     */
    virtual ~Router();

    /**
     * @brief Connects a handler for the given signal type. Replaces any existing handler
     *
     * @tparam T The type of signal to route
     * @tparam TCb Signature of the callback. Should be void(const T&)
     * @param cb The callback to invoke when a signal of type T is received
     */
    template<typename T, typename TCb>
    void route(TCb&& cb);

    /**
     * @brief Removes the callback for the given signal type
     * @tparam T The type of signal to remove the route for
     */
    template<typename T>
    void removeRoute();

    /**
     * @brief Removes all routes for all signal types
     */
    void removeRoutes();

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
Router<TSignals...>::Router() {}

template<typename... TSignals>
Router<TSignals...>::~Router() {}

template<typename... TSignals>
void Router<TSignals...>::subscribe(Channel& channel) {
    if (isSubscribed()) {
        BL_LOG_WARN << "Router already subscribed to a channel, replacing subscription";
    }

    if (channel.isShutdown()) {
        BL_LOG_ERROR << "Cannot subscribe to a shutdown channel";
        return;
    }

    (channel.getStream<TSignals>()->subscribe(static_cast<Handler<TSignals>*>(this)), ...);
};

template<typename... TSignals>
void Router<TSignals...>::subscribeDeferred(Channel& channel) {
    if (isSubscribed()) {
        BL_LOG_WARN << "Router already subscribed to a channel, replacing subscription";
    }

    if (channel.isShutdown()) {
        BL_LOG_ERROR << "Cannot subscribe to a shutdown channel";
        return;
    }

    (channel.getStream<TSignals>()->subscribeDeferred(static_cast<Handler<TSignals>*>(this)), ...);
};

template<typename... TSignals>
void Router<TSignals...>::unsubscribe() {
    if (isSubscribed()) { (static_cast<Handler<TSignals>*>(this)->unsubscribe(), ...); }
}

template<typename... TSignals>
void Router<TSignals...>::unsubscribeDeferred() {
    if (isSubscribed()) { (static_cast<Handler<TSignals>*>(this)->unsubscribeDeferred(), ...); }
}

template<typename... TSignals>
bool Router<TSignals...>::isSubscribed() const {
    return Handler<CheckSignal>::subscribedTo != nullptr;
}

template<typename... TSignals>
template<typename T, typename TCb>
void Router<TSignals...>::route(TCb&& cb) {
    this->priv::Route<T>::callback = std::forward<TCb>(cb);
}

template<typename... TSignals>
template<typename T>
void Router<TSignals...>::removeRoute() {
    this->priv::Route<T>::callback = nullptr;
}

template<typename... TSignals>
void Router<TSignals...>::removeRoutes() {
    ((this->priv::Route<TSignals>::callback = nullptr), ...);
}

} // namespace sig
} // namespace bl

#endif
