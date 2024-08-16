#ifndef BLIB_UTIL_MUTEX_HPP
#define BLIB_UTIL_MUTEX_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <thread>

namespace bl
{
namespace util
{
/**
 * @brief Reader-writer lock built on top of std::shared_mutex. The primary benefit of this class is
 *        that lockRead and lockWrite do not deadlock if called by the same thread more than once.
 *        Note that calling one and then the other will still deadlock
 *
 * @ingroup Util
 *
 */
class ReadWriteLock : private NonCopyable {
public:
    /**
     * @brief Construct a new Read Write Lock object
     *
     */
    ReadWriteLock();

    /**
     * @brief Acquires a lock for reading a shared resource. Multiple threads may acquire a read
     *        lock provided that no thread holds a write lock
     *
     */
    void lockRead();

    /**
     * @brief Acquires a lock for writing a shared resource. Only one thread may hold this lock
     *
     */
    void lockWrite();

    /**
     * @brief Releases the read lock held by this thread
     *
     */
    void unlockRead();

    /**
     * @brief Releases the write lock held by this thread
     *
     */
    void unlockWrite();

    /**
     * @brief RAII helper to acquire a read lock and release it when the scope is exited
     *
     * @ingroup Util
     *
     */
    class ReadScopeGuard : private NonCopyable {
    public:
        /**
         * @brief Construct a new Read Scope Guard object
         *
         * @param lock The lock to lock in read mode
         */
        ReadScopeGuard(ReadWriteLock& lock);

        /**
         * @brief Unlocks the underlying lock
         *
         */
        ~ReadScopeGuard();

        /**
         * @brief Prematurely unlocks the underlying lock
         *
         */
        void unlock();

    private:
        ReadWriteLock& lock;
        bool unlocked;
    };

    /**
     * @brief RAII helper to acquire a write lock and release it when the scope is exited
     *
     * @ingroup Util
     *
     */
    class WriteScopeGuard : private NonCopyable {
    public:
        /**
         * @brief Construct a new Write Scope Guard object
         *
         * @param lock The lock to lock in write mode
         */
        WriteScopeGuard(ReadWriteLock& lock);

        /**
         * @brief Unlocks the underlying lock
         *
         */
        ~WriteScopeGuard();

        /**
         * @brief Prematurely unlocks the underlying lock
         *
         */
        void unlock();

    private:
        ReadWriteLock& lock;
        bool unlocked;
    };

private:
    std::mutex mutex;
    std::condition_variable cv;
    unsigned int readerCount;
    unsigned int recursiveWriteLock;
    std::optional<std::thread::id> writeLocker;

    bool otherThreadHasWriteLock() const {
        return writeLocker.has_value() && writeLocker.value() != std::this_thread::get_id();
    }

    bool canLockWrite() const { return readerCount == 0 && !otherThreadHasWriteLock(); }
};

} // namespace util
} // namespace bl

#endif
