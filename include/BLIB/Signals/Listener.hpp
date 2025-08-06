#ifndef BLIB_SIGNALS_LISTENER_HPP
#define BLIB_SIGNALS_LISTENER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Signals/Channel.hpp>
#include <BLIB/Signals/Handler.hpp>
#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace sig
{
class Channel;

template<typename... TSignals>
class Listener : public Handler<TSignals>... {
public:
    Listener();

    virtual ~Listener();

    void subscribe(Channel& channel);

    void subscribeDeferred(Channel& channel);

    void unsubscribe();

    void unsubscribeDeferred();

    bool isSubscribed() const;

private:
    bool subscribed;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TSignals>
Listener<TSignals...>::Listener()
: subscribed(false) {}

template<typename... TSignals>
Listener<TSignals...>::~Listener() {
    unsubscribe();
}

template<typename... TSignals>
void Listener<TSignals...>::subscribe(Channel& channel) {
    if (subscribed) {
        BL_LOG_WARN << "Listener already subscribed to a channel, replacing subscription";
    }

    subscribed = true;
    (channel.getStream<TSignals>()->subscribe(static_cast<Handler<TSignals>*>(this)), ...);
};

template<typename... TSignals>
void Listener<TSignals...>::subscribeDeferred(Channel& channel) {
    if (subscribed) {
        BL_LOG_WARN << "Listener already subscribed to a channel, replacing subscription";
    }

    subscribed = true;
    (channel.getStream<TSignals>()->subscribeDeferred(static_cast<Handler<TSignals>*>(this)), ...);
};

template<typename... TSignals>
void Listener<TSignals...>::unsubscribe() {
    if (subscribed) {
        subscribed = false;
        (static_cast<Handler<TSignals>*>(this)->unsubscribe(), ...);
    }
}

template<typename... TSignals>
void Listener<TSignals...>::unsubscribeDeferred() {
    if (subscribed) {
        subscribed = false;
        (static_cast<Handler<TSignals>*>(this)->unsubscribeDeferred(), ...);
    }
}

template<typename... TSignals>
bool Listener<TSignals...>::isSubscribed() const {
    return subscribed;
}

} // namespace sig
} // namespace bl

#endif
