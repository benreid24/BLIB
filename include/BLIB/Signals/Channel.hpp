#ifndef BLIB_SIGNALS_CHANNEL_HPP
#define BLIB_SIGNALS_CHANNEL_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Signals/Stream.hpp>
#include <BLIB/Util/NonCopyable.hpp>
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
}

class Channel : private util::NonCopyable {
public:
    Channel();

    Channel(Channel&& channel);

    ~Channel();

    Channel& operator=(Channel&& channel);

    template<typename TSignal>
    Stream<TSignal>* getStream();

private:
    std::mutex mutex;
    std::unordered_map<std::type_index, priv::StreamBase*> streams;
    ctr::FastEraseVector<priv::EmitterBase*> emitters;
    // TODO - track subscribed listeners

    friend class priv::EmitterBase;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TSignal>
Stream<TSignal>* Channel::getStream() {
    const std::type_index key(typeid(TSignal));
    auto it = streams.find(key);
    if (it == streams.end()) { it = streams.try_emplace(key, new Stream<TSignal>()).first; }
    return static_cast<Stream<TSignal>*>(it->second);
}

} // namespace sig
} // namespace bl

#endif
