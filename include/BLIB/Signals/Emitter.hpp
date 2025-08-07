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
/**
 * @brief Signal emitter that connects to a channel and allows the sending of signals
 *
 * @tparam ...TSignals The types of signals that this emitter can send
 * @ingroup Signals
 */
template<typename... TSignals>
class Emitter : public priv::EmitterBase {
public:
    /**
     * @brief Creates a new emitter
     */
    Emitter();

    /**
     * @brief Creates a new emitter and connects to the same channel as another emitter. Both
     *        emitters will be connected to the channel
     *
     * @param other The emitter to copy the channel connection from
     */
    Emitter(const Emitter& other);

    /**
     * @brief Creates a new emitter and adopts the channel connection of another emitter
     *
     * @param other The emitter to adopt the channel connection from
     */
    Emitter(Emitter&& other);

    /**
     * @brief Destroys the emitter and disconnects from the channel if connected
     */
    ~Emitter();

    /**
     * @brief Creates a new emitter and connects to the same channel as another emitter. Both
     *        emitters will be connected to the channel
     *
     * @param other The emitter to copy the channel connection from
     * @return A reference to this emitter
     */
    Emitter& operator=(const Emitter& other);

    /**
     * @brief Creates a new emitter and adopts the channel connection of another emitter
     *
     * @param other The emitter to adopt the channel connection from
     * @return A reference to this emitter
     */
    Emitter& operator=(Emitter&& other);

    /**
     * @brief Connects the emitter to the given channel and disconnects from any previous. This must
     *        be called before any signals can be sent
     *
     * @param channel The channel to connect to
     */
    void connect(Channel& channel);

    /**
     * @brief Disconnects the emitter from the channel it was connected to
     */
    virtual void disconnect() override;

    /**
     * @brief Emits a signal of the given type to the connected channel. T must be one of TSignals
     *
     * @tparam T The type of signal to emit
     * @param signal The signal to emit
     */
    template<typename T>
    void emit(const T& signal);

    /**
     * @brief Emits a signal of the given type to the connected channel. T must be one of TSignals.
     *        Locks the underlying stream before emit. Use this if multiple threads may be
     *        subscribing or unsubscribing to the stream while others emit signals
     *
     * @tparam T The type of signal to emit
     * @param signal The signal to emit
     */
    template<typename T>
    void emitSynchronized(const T& signal);

    /**
     * @brief Returns whether the emitter is connected to a channel
     */
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
