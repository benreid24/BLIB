#include <BLIB/Signals/Priv/EmitterBase.hpp>

#include <BLIB/Signals/Priv/StreamBase.hpp>

namespace bl
{
namespace sig
{
namespace priv
{
void EmitterBase::registerWithStreams(StreamBase** streams, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        std::unique_lock lock(streams[i]->emitterMutex);
        streams[i]->emitters.push_back(this);
    }
}

void EmitterBase::replaceStreamRegistrations(StreamBase** streams, std::size_t n,
                                             EmitterBase* old) {
    for (std::size_t i = 0; i < n; ++i) {
        std::unique_lock lock(streams[i]->emitterMutex);
        for (auto it = streams[i]->emitters.begin(); it != streams[i]->emitters.end(); ++it) {
            if (*it == old) {
                *it = this;
                break;
            }
        }
    }
}

void EmitterBase::removeStreamRegistrations(StreamBase** streams, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        std::unique_lock lock(streams[i]->emitterMutex);
        for (auto it = streams[i]->emitters.begin(); it != streams[i]->emitters.end(); ++it) {
            if (*it == this) {
                streams[i]->emitters.erase(it);
                break;
            }
        }
    }
}

} // namespace priv
} // namespace sig
} // namespace bl
