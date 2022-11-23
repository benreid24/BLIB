#include <BLIB/Util/Waiter.hpp>

#include <unordered_set>

namespace bl
{
namespace util
{
namespace
{
std::unordered_set<Waiter*> table;
std::mutex tableMutex;
std::atomic_bool ended = false;
} // namespace

Waiter::Waiter()
: unblocked(false) {
    std::unique_lock lock(tableMutex);
    table.insert(this);
}

Waiter::~Waiter() {
    {
        std::unique_lock lock(tableMutex);
        table.erase(this);
    }
    unblock();
}

void Waiter::unblock() {
    unblocked = true;
    waitVar.notify_all();
}

bool Waiter::wasUnblocked() const { return unblocked; }

void Waiter::wait() {
    if (allUnblocked()) {
        BL_LOG_ERROR << "wait() called during shutdown. Threads should be exiting";
        return;
    }
    if (!unblocked) {
        std::unique_lock lock(waitMutex);
        if (!unblocked) { waitVar.wait(lock); }
    }
}

void Waiter::reset() { unblocked = false; }

void Waiter::unblockAll() {
    ended = true;

    const auto checkDone = []() -> bool {
        std::unique_lock lock(tableMutex);
        return table.empty();
    };
    while (!checkDone()) {
        std::unordered_set<Waiter*>::iterator it;
        {
            std::unique_lock lock(tableMutex);
            if (table.empty()) break;
            it = table.begin();
        }
        (*it)->unblock();
        {
            std::unique_lock lock(tableMutex);
            table.erase(it);
        }
    }
}

bool Waiter::allUnblocked() { return ended; }

} // namespace util
} // namespace bl
