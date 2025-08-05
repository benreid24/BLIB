#include <BLIB/Signals/Channel.hpp>

#include <BLIB/Signals/Emitter.hpp>

namespace bl
{
namespace sig
{
Channel::Channel()
: needDeferSync(false)
, streams()
, emitters() {}

Channel::Channel(Channel&& channel)
: streams(std::move(channel.streams))
, emitters(std::move(channel.emitters)) {}

Channel::~Channel() {
    for (auto& emitter : emitters) { emitter->disconnect(); }
    for (auto& stream : streams) { delete stream.second; }
}

Channel& Channel::operator=(Channel&& channel) {
    streams  = std::move(channel.streams);
    emitters = std::move(channel.emitters);
    return *this;
}

void Channel::syncDeferred() {
    if (needDeferSync) {
        needDeferSync = false;
        std::unique_lock lock(mutex);
        for (auto& stream : streams) { stream.second->syncDeferred(); }
    }
}

} // namespace sig
} // namespace bl
