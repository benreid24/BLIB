#ifndef BLIB_SIGNALS_CHANNEL_HPP
#define BLIB_SIGNALS_CHANNEL_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Signals/Stream.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <mutex>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace sig
{
namespace priv
{
class EmitterBase;
class ListenerBase;
} // namespace priv

class Channel : private util::NonCopyable {
public:
    Channel();

    Channel(Channel&& channel);

    ~Channel();

    Channel& operator=(Channel&& channel);

    template<typename TSignal>
    Stream<TSignal>* getStream();

    void syncDeferred();

private:
    std::mutex mutex;
    std::atomic_bool needDeferSync;
    std::unordered_map<std::type_index, priv::StreamBase*> streams;
    ctr::FastEraseVector<priv::EmitterBase*> emitters;

    friend class priv::EmitterBase;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TSignal>
Stream<TSignal>* Channel::getStream() {
    needDeferSync = true;
    const std::type_index key(typeid(TSignal));
    auto it = streams.find(key);
    if (it == streams.end()) { it = streams.try_emplace(key, new Stream<TSignal>()).first; }
    return static_cast<Stream<TSignal>*>(it->second);
}

} // namespace sig
} // namespace bl

#endif
