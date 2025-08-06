#ifndef BLIB_SIGNALS_EMITTER_HPP
#define BLIB_SIGNALS_EMITTER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Signals/Channel.hpp>
#include <BLIB/Signals/Priv/EmitterBase.hpp>
#include <BLIB/Signals/Stream.hpp>
#include <array>
#include <tuple>

namespace bl
{
namespace sig
{
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
    void emit(const T& signal);

    template<typename T>
    void emitSynchronized(const T& signal);

    bool isConnected() const;

private:
    bool connected;
    std::tuple<Stream<TSignals>*...> streams;

    void registerWithStreams();
    void replaceStreamRegistrations(Emitter* other);
    void removeStreamRegistrations();
    std::array<priv::StreamBase*, sizeof...(TSignals)> getStreams();
    virtual void disconnectForStreamDesctruction() override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TSignals>
Emitter<TSignals...>::Emitter()
: connected(false)
, streams{} {}

template<typename... TSignals>
Emitter<TSignals...>::Emitter(const Emitter& other)
: connected(other.connected)
, streams(other.streams) {
    registerWithStreams();
}

template<typename... TSignals>
Emitter<TSignals...>::Emitter(Emitter&& other)
: connected(other.connected)
, streams(other.streams) {
    replaceStreamRegistrations(&other);
    other.connected = false;
}

template<typename... TSignals>
Emitter<TSignals...>::~Emitter() {
    disconnect();
}

template<typename... TSignals>
Emitter<TSignals...>& Emitter<TSignals...>::operator=(const Emitter& other) {
    connected = other.connected;
    streams   = other.streams;
    registerWithStreams();
    return *this;
}

template<typename... TSignals>
Emitter<TSignals...>& Emitter<TSignals...>::operator=(Emitter&& other) {
    connected = other.connected;
    streams   = other.streams;
    replaceStreamRegistrations(&other);
    other.connected = false;
    return *this;
}

template<typename... TSignals>
void Emitter<TSignals...>::connect(Channel& channel) {
    disconnect();

    connected = true;
    std::apply([&channel](Stream<TSignals>*&... s) { ((s = channel.getStream<TSignals>()), ...); },
               streams);
    registerWithStreams();
}

template<typename... TSignals>
void Emitter<TSignals...>::disconnect() {
    if (connected) {
        removeStreamRegistrations();
        disconnectForStreamDesctruction();
    }
}

template<typename... TSignals>
bool Emitter<TSignals...>::isConnected() const {
    return connected;
}

template<typename... TSignals>
void Emitter<TSignals...>::registerWithStreams() {
    if (connected) {
        auto streamArray = getStreams();
        EmitterBase::registerWithStreams(streamArray.data(), streamArray.size());
    }
}

template<typename... TSignals>
void Emitter<TSignals...>::replaceStreamRegistrations(Emitter* other) {
    if (connected) {
        auto streamArray = getStreams();
        EmitterBase::replaceStreamRegistrations(streamArray.data(), streamArray.size(), other);
    }
}

template<typename... TSignals>
void Emitter<TSignals...>::removeStreamRegistrations() {
    if (connected) {
        auto streamArray = getStreams();
        EmitterBase::removeStreamRegistrations(streamArray.data(), streamArray.size());
    }
}

template<typename... TSignals>
std::array<priv::StreamBase*, sizeof...(TSignals)> Emitter<TSignals...>::getStreams() {
    std::array<priv::StreamBase*, sizeof...(TSignals)> result{};
    unsigned int i = 0;
    std::apply([&result, &i](Stream<TSignals>*... s) { ((result[i++] = s), ...); }, streams);
    return result;
}

template<typename... TSignals>
void Emitter<TSignals...>::disconnectForStreamDesctruction() {
    connected = false;
    std::apply([](Stream<TSignals>*&... s) { ((s = nullptr), ...); }, streams);
}

template<typename... TSignals>
template<typename T>
void Emitter<TSignals...>::emit(const T& signal) {
    if (connected) {
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
void Emitter<TSignals...>::emitSynchronized(const T& signal) {
    if (connected) {
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
