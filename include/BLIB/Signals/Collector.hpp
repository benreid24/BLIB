#ifndef BLIB_SIGNALS_COLLECTOR_HPP
#define BLIB_SIGNALS_COLLECTOR_HPP

#include <BLIB/Signals/Channel.hpp>
#include <BLIB/Signals/Handler.hpp>
#include <mutex>
#include <vector>

namespace bl
{
namespace sig
{
/**
 * @brief Signal collector that collects signals of a single type and allows for draining them later
 *
 * @tparam TSignal The type of signal to collect
 * @ingroup Signals
 */
template<typename TSignal>
class Collector : public Handler<TSignal> {
public:
    /**
     * @brief Creates a new collector with an optional capacity hint for the internal capacity
     *
     * @param capacityHint The expected number of signals to collect
     */
    Collector(std::size_t capacityHint = 0);

    /**
     * @brief Calls a given callback for each collected signal and then clears the queue
     *
     * @tparam TCb The type of callback. Should be void(const TSignal&)
     * @param cb The callback to invoke for each collected signal
     */
    template<typename TCb>
    void drain(TCb&& cb);

    /**
     * @brief Calls a given callback for each collected signal and then clears the queue. Use this
     *        if it is expected that other threads may send more signals while draining
     *
     * @tparam TCb The type of callback. Should be void(const TSignal&)
     * @param cb The callback to invoke for each collected signal
     */
    template<typename TCb>
    void drainSync(TCb&& cb);

    /**
     * @brief Like drainSync but performs a move of the signal queue to minimize lock time
     *
     * @tparam TCb The type of callback. Should be void(const TSignal&)
     * @param cb The callback to invoke for each collected signal
     */
    template<typename TCb>
    void drainSyncFast(TCb&& cb);

    /**
     * @brief Subscribes the collector to the given channel
     *
     * @param channel The channel to subscribe to
     */
    void subscribe(Channel& channel);

    /**
     * @brief Defers subscription to the given channel. Use this if calling from a signal handler
     *
     * @param channel The channel to subscribe to
     */
    void subscribeDeferred(Channel& channel);

    /**
     * @brief Unsubscribes the collector from the connected channel
     */
    using Handler<TSignal>::unsubscribe;

    /**
     * @brief Unsubscribes the collector from the connected channel but defers the unsubscribe until
     *        the start of the next frame. Use this if calling from a signal handler
     */
    using Handler<TSignal>::unsubscribeDeferred;

    /**
     * @brief Returns whether the collector is currently subscribed to a channel
     */
    bool isSubscribed() const;

private:
    std::mutex mutex;
    std::vector<TSignal> collected;

    virtual void process(const TSignal& signal) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TSignal>
Collector<TSignal>::Collector(std::size_t capacityHint) {
    collected.reserve(capacityHint);
}

template<typename TSignal>
template<typename TCb>
void Collector<TSignal>::drain(TCb&& cb) {
    for (const TSignal& signal : collected) { cb(signal); }
    collected.clear();
}

template<typename TSignal>
template<typename TCb>
void Collector<TSignal>::drainSync(TCb&& cb) {
    std::unique_lock lock(mutex);
    for (const TSignal& signal : collected) { cb(signal); }
    collected.clear();
}

template<typename TSignal>
template<typename TCb>
void Collector<TSignal>::drainSyncFast(TCb&& cb) {
    std::unique_lock lock(mutex);
    std::vector<TSignal> toProcess = std::move(collected);
    lock.unlock();
    for (const TSignal& signal : toProcess) { cb(signal); }
}

template<typename TSignal>
void Collector<TSignal>::subscribe(Channel& channel) {
    channel.getStream<TSignal>()->subscribe(this);
}

template<typename TSignal>
void Collector<TSignal>::subscribeDeferred(Channel& channel) {
    channel.getStream<TSignal>()->subscribeDeferred(this);
}

template<typename TSignal>
bool Collector<TSignal>::isSubscribed() const {
    return this->subscribedTo != nullptr;
}

template<typename TSignal>
void Collector<TSignal>::process(const TSignal& signal) {
    std::unique_lock lock(mutex);
    collected.emplace_back(signal);
}

} // namespace sig
} // namespace bl

#endif
