#include <BLIB/Signals/Channel.hpp>

#include <BLIB/Signals/Emitter.hpp>

namespace bl
{
namespace sig
{
Channel::Channel()
: streams()
, emitters() {}

Channel::Channel(Channel&& channel)
: streams(std::move(channel.streams))
, emitters(std::move(channel.emitters)) {
    for (auto& emitter : emitters) { emitter->connectedTo = this; }
}

Channel::~Channel() {
    for (auto& emitter : emitters) { emitter->disconnect(); }
    for (auto& stream : streams) { delete stream.second; }
}

Channel& Channel::operator=(Channel&& channel) {
    streams  = std::move(channel.streams);
    emitters = std::move(channel.emitters);
    for (auto& emitter : emitters) { emitter->connectedTo = this; }
    return *this;
}

} // namespace sig
} // namespace bl
