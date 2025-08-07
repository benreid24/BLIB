#ifndef BLIB_SIGNALS_HANDLER_HPP
#define BLIB_SIGNALS_HANDLER_HPP

#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace sig
{
template<typename T>
class Stream;

/**
 * @brief Base class for signal listeners that handles a single type of signal. Not intended for
 *        direct use, use Listener instead
 *
 * @tparam T The type of signal to process
 * @ingroup Signals
 */
template<typename T>
class Handler : private util::NonCopyable {
public:
    /**
     * @brief Initializes the handler
     */
    Handler()
    : subscribedTo(nullptr) {}

    /**
     * @brief Unsubscribes from the stream and destroys the handler
     */
    virtual ~Handler();

    /**
     * @brief Derived classes must implement this method to process the signal
     *
     * @param signal The signal to process
     */
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
