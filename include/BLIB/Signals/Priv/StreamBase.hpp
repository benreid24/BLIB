#ifndef BLIB_SIGNALS_PRIV_STREAMBASE_HPP
#define BLIB_SIGNALS_PRIV_STREAMBASE_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <mutex>

namespace bl
{
namespace sig
{
namespace priv
{
class EmitterBase;

class StreamBase {
public:
    virtual ~StreamBase();

    virtual void syncDeferred() = 0;

private:
    std::mutex emitterMutex;
    ctr::FastEraseVector<EmitterBase*> emitters;

    friend class EmitterBase;
};

} // namespace priv
} // namespace sig
} // namespace bl

#endif
