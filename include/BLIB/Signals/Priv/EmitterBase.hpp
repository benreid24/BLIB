#ifndef BLIB_SIGNALS_PRIV_EMITTERBASE_HPP
#define BLIB_SIGNALS_PRIV_EMITTERBASE_HPP

#include <cstddef>

namespace bl
{
namespace sig
{
/// Implementation detail namespace for the Signals module
namespace priv
{
class StreamBase;

class EmitterBase {
public:
    virtual void disconnect() = 0;

protected:
    void registerWithStreams(StreamBase** streams, std::size_t n);
    void replaceStreamRegistrations(StreamBase** streams, std::size_t n, EmitterBase* old);
    void removeStreamRegistrations(StreamBase** streams, std::size_t n);

private:
    virtual void disconnectForStreamDesctruction() = 0;

    friend class StreamBase;
};

} // namespace priv
} // namespace sig
} // namespace bl

#endif
