#include <BLIB/Signals/Priv/StreamBase.hpp>

#include <BLIB/Signals/Priv/EmitterBase.hpp>

namespace bl
{
namespace sig
{
namespace priv
{
StreamBase::~StreamBase() {
    for (EmitterBase* emitter : emitters) { emitter->disconnect(); }
}

} // namespace priv
} // namespace sig
} // namespace bl
