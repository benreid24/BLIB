#ifndef BLIB_SIGNALS_HANDLER_HPP
#define BLIB_SIGNALS_HANDLER_HPP

#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace sig
{
template<typename T>
class Stream;

template<typename T>
class Handler : private util::NonCopyable {
public:
    Handler()
    : subscribedTo(nullptr) {}

    virtual ~Handler();

    virtual void process(const T& signal) = 0;

protected:
    Stream<T>* subscribedTo;

    void unsubscribe();
    void unsubscribeDeferred();

    template<typename...>
    friend class Listener;
    friend class Stream<T>;
};

} // namespace sig
} // namespace bl

#endif
