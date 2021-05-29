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

void Waiter::wait() {
    if (!unblocked) {
        std::unique_lock lock(waitMutex);
        waitVar.wait(lock);
    }
}

void Waiter::reset() { unblocked = false; }

void Waiter::unblockAll() {
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

} // namespace util
} // namespace bl
