#ifndef BLIB_SIGNALS_DISPATCHER_HPP
#define BLIB_SIGNALS_DISPATCHER_HPP

#include <BLIB/Signals/Channel.hpp>
#include <BLIB/Signals/Stream.hpp>
#include <tuple>

namespace bl
{
namespace sig
{
template<typename... TSignals>
class Dispatcher {
public:
    //

private:
    Channel* connectedTo;
    std::tuple<Stream<TSignals>...> streams;
};

} // namespace sig
} // namespace bl

#endif
