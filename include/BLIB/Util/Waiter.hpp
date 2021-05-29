#ifndef BLIB_UTIL_WAITER_HPP
#define BLIB_UTIL_WAITER_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace bl
{
namespace util
{
/**
 * @brief Basic utility class to pause a thread while waiting for a specific event
 *
 * @ingroup Util
 *
 */
class Waiter {
public:
    /**
     * @brief Construct a new Waiter object
     *
     */
    Waiter();

    /**
     * @brief Unblocks the waiting thread
     *
     */
    void unblock();

    /**
     * @brief Resets the waiter to allow it to be resued after unblock() is called
     *
     */
    void reset();

    /**
     * @brief Blocks until unblock() is called. Immediately returns if unblock() was already called
     *        and reset() was not called
     *
     */
    void wait();

private:
    std::atomic<bool> unblocked;
    std::mutex waitMutex;
    std::condition_variable waitVar;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline Waiter::Waiter()
: unblocked(false) {}

inline void Waiter::unblock() {
    unblocked = true;
    waitVar.notify_all();
}

inline void Waiter::wait() {
    if (!unblocked) {
        std::unique_lock lock(waitMutex);
        waitVar.wait(lock);
    }
}

inline void Waiter::reset() { unblocked = false; }

} // namespace util
} // namespace bl

#endif
