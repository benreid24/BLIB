#include <BLIB/Resources/GarbageCollector.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Resources/Manager.hpp>
#include <optional>
#include <vector>

namespace bl
{
namespace resource
{
namespace
{
using MP = std::pair<ManagerBase*, unsigned int>;

std::optional<std::thread> thread;
std::mutex threadLock;
std::atomic_bool quitFlag = false;
std::condition_variable quitCv;
std::mutex managerLock;
std::vector<MP> managers;
unsigned int nextToClean = 0;

void backgroundCleaner();
} // namespace

GarbageCollector::GarbageCollector() {
    std::unique_lock lock(threadLock);

    if (!thread.has_value() || !thread.value().joinable()) {
        thread.emplace(&backgroundCleaner);
        BL_LOG_INFO << "GarbageCollector online";
    }
    else {
        BL_LOG_ERROR << "Attempted to start a second GarbageCollector";
    }
}

GarbageCollector::~GarbageCollector() {
    std::unique_lock lock(threadLock);
    if (thread.has_value() && thread.value().joinable()) {
        BL_LOG_INFO << "Terminating GarbageCollector";
        quitFlag = true;
        quitCv.notify_all();
        thread.value().join();
        BL_LOG_INFO << "GarbageCollector terminated";
    }
    else {
        BL_LOG_ERROR << "Attempted to stop GarbageCollector that was not running";
    }
}

void GarbageCollector::registerManager(ManagerBase* m) {
    std::unique_lock lock(managerLock);

    // try to find a spot to insert
    for (auto it = managers.begin(); it != managers.end(); ++it) {
        if (it->second > m->gcPeriod) {
            managers.insert(it, MP(m, m->gcPeriod));
            return;
        }
    }

    managers.emplace_back(m, m->gcPeriod);
}

void GarbageCollector::unregisterManager(ManagerBase* m) {
    std::unique_lock lock(managerLock);

    for (unsigned int i = 0; i < managers.size(); ++i) {
        if (managers[i].first == m) {
            if (i < nextToClean) { nextToClean -= 1; }
            managers.erase(managers.begin() + i);
            break;
        }
    }
}

namespace
{
struct Sorter {
    inline bool operator()(const MP& left, const MP& right) { return left.second < right.second; }
};

void backgroundCleaner() {
    // perform one time setup
    quitFlag = false;
    {
        std::unique_lock lock(managerLock);
        for (auto& mp : managers) { mp.second = mp.first->gcPeriod; }
        std::sort(managers.begin(), managers.end(), Sorter());
        nextToClean = 0;
    }

    BL_LOG_INFO << "gc setup and running";

    while (!quitFlag) {
        std::unique_lock lock(managerLock);
        BL_LOG_INFO << "gc loop start";
        if (managers.empty()) {
            quitCv.wait_for(lock, std::chrono::seconds(60));
            continue;
        }
        if (quitFlag) return;
        BL_LOG_INFO << "gc past empty check";

        const unsigned int sleptFor = managers[nextToClean].second;
        BL_LOG_INFO << "gc sleeping for " << sleptFor;
        quitCv.wait_for(lock, std::chrono::seconds(sleptFor));
        BL_LOG_INFO << "gc awake";
        if (quitFlag) return;
        if (managers.empty()) continue;

        BL_LOG_INFO << "cleaning manager " << nextToClean << ": " << managers[nextToClean].first;
        managers[nextToClean].first->doClean();
        managers[nextToClean].second = managers[nextToClean].first->gcPeriod;
        BL_LOG_INFO << "cleaned";

        // proceed through others that also should clean
        const unsigned int start = nextToClean;
        unsigned int i           = (nextToClean + 1) % managers.size();
        while (i != start) {
            if (managers[i].second <= sleptFor) {
                managers[i].first->doClean();
                managers[i].second = managers[i].first->gcPeriod;
            }
            else {
                managers[i].second -= sleptFor;
                nextToClean = i;
            }
            i = (i + 1) % managers.size();
        }

        BL_LOG_INFO << "next to clean is " << nextToClean;

        // re-sort now that times have reset
        std::sort(managers.begin(), managers.end(), Sorter());
        BL_LOG_INFO << "sorted";
    }
}
} // namespace

} // namespace resource
} // namespace bl
