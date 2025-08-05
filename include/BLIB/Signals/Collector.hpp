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
template<typename TSignal>
class Collector : public Handler<TSignal> {
public:
    Collector(std::size_t capacityHint = 0);

    template<typename TCb>
    void drain(TCb&& cb);

    template<typename TCb>
    void drainSync(TCb&& cb);

    template<typename TCb>
    void drainSyncFast(TCb&& cb);

    void subscribe(Channel& channel);

    void subscribeDeferred(Channel& channel);

    using Handler::unsubscribe;

    using Handler::unsubscribeDeferred;

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
