#include <BLIB/Util/ReadWriteLock.hpp>

#include <unordered_map>

namespace bl
{
namespace util
{
namespace
{
thread_local std::unordered_map<ReadWriteLock*, bool>* readLockFlags =
    new std::unordered_map<ReadWriteLock*, bool>();
thread_local std::unordered_map<ReadWriteLock*, bool>* writeLockFlags =
    new std::unordered_map<ReadWriteLock*, bool>();
} // namespace

void ReadWriteLock::lockRead() {
    if (readLockFlag()) return;
    readLockFlag() = true;
    mutex.lock_shared();
}

void ReadWriteLock::unlockRead() {
    if (!readLockFlag()) return;
    readLockFlag() = false;
    mutex.unlock_shared();
}

void ReadWriteLock::lockWrite() {
    if (writeLockFlag()) return;
    writeLockFlag() = true;
    mutex.lock();
}

void ReadWriteLock::unlockWrite() {
    if (!writeLockFlag()) return;
    writeLockFlag() = false;
    mutex.unlock();
}

bool& ReadWriteLock::readLockFlag() { return (*readLockFlags)[this]; }

bool& ReadWriteLock::writeLockFlag() { return (*writeLockFlags)[this]; }

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
