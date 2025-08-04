#ifndef BLIB_SIGNALS_DISPATCHER_HPP
#define BLIB_SIGNALS_DISPATCHER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Signals/Channel.hpp>
#include <BLIB/Signals/Stream.hpp>
#include <tuple>

namespace bl
{
namespace sig
{
/// Implementation detail namespace for the Signals module
namespace priv
{
class EmitterBase {
public:
    virtual void disconnect() = 0;

protected:
    Channel* connectedTo;

    void registerWithChannel() {
        if (connectedTo) {
            std::unique_lock lock(connectedTo->mutex);
            registerWithChannelUnlocked();
        }
    }

    void registerWithChannelUnlocked() {
        if (connectedTo) { connectedTo->emitters.emplace_back(this); }
    }

    void replaceRegistration(EmitterBase* oldEmitter) {
        if (connectedTo) {
            std::unique_lock lock(connectedTo->mutex);

            for (auto it = connectedTo->emitters.begin(); it != connectedTo->emitters.end(); ++it) {
                if (*it == oldEmitter) {
                    *it = this;
                    return;
                }
            }

            // register if not found
            registerWithChannelUnlocked();
        }
    };

    void unregisterFromChannel() {
        if (connectedTo) {
            std::unique_lock lock(connectedTo->mutex);
            unregisterFromChannelUnlocked();
        }
    }

    void unregisterFromChannelUnlocked() {
        if (connectedTo) {
            for (auto it = connectedTo->emitters.begin(); it != connectedTo->emitters.end(); ++it) {
                if (*it == this) {
                    connectedTo->emitters.erase(it);
                    return;
                }
            }
        }
    }

    std::unique_lock<std::mutex>&& makeLock() {
        return std::unique_lock<std::mutex>(connectedTo->mutex);
    }

    friend class Channel;
};
} // namespace priv

template<typename... TSignals>
class Emitter : public priv::EmitterBase {
public:
    Emitter();

    Emitter(const Emitter& other);

    Emitter(Emitter&& other);

    ~Emitter();

    Emitter& operator=(const Emitter& other);

    Emitter& operator=(Emitter&& other);

    void connect(Channel& channel);

    virtual void disconnect() override;

    template<typename T>
    void signal(const T& signal);

    template<typename T>
    void signalSynchronized(const T& signal);

private:
    std::tuple<Stream<TSignals>*...> streams;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TSignals>
Emitter<TSignals...>::Emitter()
: connectedTo(nullptr)
, streams{} {}

template<typename... TSignals>
Emitter<TSignals...>::Emitter(const Emitter& other)
: connectedTo(other.connectedTo)
, streams(other.streams) {
    registerWithChannel();
}

template<typename... TSignals>
Emitter<TSignals...>::Emitter(Emitter&& other)
: connectedTo(other.connectedTo)
, streams(others.streams) {
    replaceRegistration(&other);
    other.connectedTo = nullptr;
}

template<typename... TSignals>
Emitter<TSignals...>::~Emitter() {
    disconnect();
}

template<typename... TSignals>
Emitter<TSignals...>& Emitter<TSignals...>::operator=(const Emitter& other) {
    connectedTo = other.connectedTo;
    streams     = other.streams;
    registerWithChannel();
    return *this;
}

template<typename... TSignals>
Emitter<TSignals...>& Emitter<TSignals...>::operator=(Emitter&& other) {
    connectedTo = other.connectedTo;
    streams     = other.streams;
    replaceRegistration(&other);
    other.connectedTo = nullptr;
    return *this;
}

template<typename... TSignals>
void Emitter<TSignals...>::connect(Channel& channel) {
    disconnect();

    std::unique_lock lock(makeLock());
    connectedTo = &channel;
    std::apply([&channel](Stream<TSignals>*... s) { ((s = channel.getStream<TSignals>()), ...); },
               streams);
    registerWithChannelUnlocked();
}

template<typename... TSignals>
void Emitter<TSignals...>::disconnect() {
    if (connectedTo) {
        std::unique_lock lock(makeLock());
        unregisterFromChannelUnlocked();
        connectedTo = nullptr;
        std::apply([](Stream<TSignals>*... s) { ((s = nullptr), ...); }, streams);
    }
}

template<typename... TSignals>
template<typename T>
void Emitter<TSignals...>::signal(const T& signal) {
    if (connectedTo) {
        if constexpr (std::disjunction_v<std::is_same<T, TSignals>...>) {
            auto& stream = std::get<Stream<T>*>(streams);
            stream->signal(signal);
        }
        else {
            static_assert(std::disjunction_v<std::is_same<T, TSignals>...>,
                          "Signal type not found in Emitter's signal types");
        }
    }
    else { BL_LOG_WARN << "Emitter signaled without a channel connection"; }
}

template<typename... TSignals>
template<typename T>
void Emitter<TSignals...>::signalSynchronized(const T& signal) {
    if (connectedTo) {
        if constexpr (std::disjunction_v<std::is_same<T, TSignals>...>) {
            auto& stream = std::get<Stream<T>*>(streams);
            stream->signalSynchronized(signal);
        }
        else {
            static_assert(std::disjunction_v<std::is_same<T, TSignals>...>,
                          "Signal type not found in Emitter's signal types");
        }
    }
    else { BL_LOG_WARN << "Emitter signaled without a channel connection"; }
}

} // namespace sig
} // namespace bl

#endif
