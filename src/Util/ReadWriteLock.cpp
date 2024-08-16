#include <BLIB/Util/ReadWriteLock.hpp>

namespace bl
{
namespace util
{
ReadWriteLock::ReadWriteLock()
: readerCount(0)
, recursiveWriteLock(0) {}

void ReadWriteLock::lockRead() {
    std::unique_lock lock(mutex);

    if (otherThreadHasWriteLock()) {
        cv.wait(lock, [this]() { return !otherThreadHasWriteLock(); });
    }

    ++readerCount;
}

void ReadWriteLock::unlockRead() {
    std::unique_lock lock(mutex);
#ifdef BLIB_DEBUG
    if (readerCount == 0) { throw std::runtime_error("Too many calls to unlockRead()"); }
#endif

    --readerCount;

    lock.unlock();
    cv.notify_one();
}

void ReadWriteLock::lockWrite() {
    std::unique_lock lock(mutex);

    if (!canLockWrite()) {
        cv.wait(lock, [this]() { return !canLockWrite(); });
    }

    writeLocker = std::this_thread::get_id();
    ++recursiveWriteLock;
}

void ReadWriteLock::unlockWrite() {
    std::unique_lock lock(mutex);

#ifdef BLIB_DEBUG
    if (!writeLocker.has_value() || writeLocker.value() != std::this_thread::get_id() ||
        recursiveWriteLock == 0) {
        throw std::runtime_error("Invalid call to unlockWrite()");
    }
#endif

    if (--recursiveWriteLock == 0) { writeLocker.reset(); }

    lock.unlock();
    cv.notify_one();
}

ReadWriteLock::ReadScopeGuard::ReadScopeGuard(ReadWriteLock& lock)
: lock(lock)
, unlocked(false) {
    lock.lockRead();
}

ReadWriteLock::ReadScopeGuard::~ReadScopeGuard() { unlock(); }

void ReadWriteLock::ReadScopeGuard::unlock() {
    if (!unlocked) {
        unlocked = true;
        lock.unlockRead();
    }
}

ReadWriteLock::WriteScopeGuard::WriteScopeGuard(ReadWriteLock& lock)
: lock(lock)
, unlocked(false) {
    lock.lockWrite();
}

ReadWriteLock::WriteScopeGuard::~WriteScopeGuard() { unlock(); }

void ReadWriteLock::WriteScopeGuard::unlock() {
    if (!unlocked) {
        unlocked = true;
        lock.unlockWrite();
    }
}

} // namespace util
} // namespace bl
