#ifndef BLIB_UTIL_WAITER_HPP
#define BLIB_UTIL_WAITER_HPP

#include <BLIB/Logging.hpp>
#include <atomic>
#include <chrono>
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
     * @brief Unblocks any waiting threads before destruction
     *
     */
    ~Waiter();

    /**
     * @brief Unblocks the waiting thread
     *
     */
    void unblock();

    /**
     * @brief Returns whether or not the Waiter has been unblocked
     *
     */
    bool wasUnblocked() const;

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

    /**
     * @brief Waits for a given amount of time then returns, even if not unblocked
     *
     * @param timeout The maximum time to wait for
     *
     */
    template<class Rep, class Period>
    void waitFor(const std::chrono::duration<Rep, Period>& timeout);

    /**
     * @brief Unblocks all waiting threads. A program leveraging the Waiter class should call this
     *        before exit. Threads utilizing the Waiter should be able to exit cleanly once
     *        unblocked
     *
     */
    static void unblockAll();

    /**
     * @brief Returns whether or not unblockAll() has been called. This is to signal threads to
     *        avoid calling wait() again during shutdown.
     *
     * @return True if unblockAll() was called and threads should wait, false otherwise
     */
    static bool allUnblocked();

private:
    std::atomic<bool> unblocked;
    std::mutex waitMutex;
    std::condition_variable waitVar;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<class Rep, class Period>
void Waiter::waitFor(const std::chrono::duration<Rep, Period>& timeout) {
    if (allUnblocked()) {
        BL_LOG_ERROR << "waitFor() called during shutdown. Threads should be exiting";
        return;
    }
    if (!unblocked) {
        std::unique_lock lock(waitMutex);
        if (!unblocked) { waitVar.wait_for(lock, timeout); }
    }
}

} // namespace util
} // namespace bl

#endif
