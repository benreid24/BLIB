#ifndef BLIB_SIGNALS_CHANNEL_HPP
#define BLIB_SIGNALS_CHANNEL_HPP

#include <BLIB/Signals/Stream.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace sig
{
class Channel : private util::NonCopyable {
public:
    Channel();

    Channel(Channel&& channel);

    ~Channel();

    Channel& operator=(Channel&& channel);

    template<typename TSignal>
    Stream<TSignal>* getStream();

    // TODO - make base class for single dispatcher?

private:
    std::mutex mutex;
    std::unordered_map<std::type_index, void*> streams;
};

} // namespace sig
} // namespace bl

#endif
