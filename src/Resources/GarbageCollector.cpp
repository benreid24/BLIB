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
struct Sorter {
    inline bool operator()(const MP& left, const MP& right) { return left.second < right.second; }
};

std::atomic_bool started = false;
std::atomic_bool stopped = false;
} // namespace

GarbageCollector::GarbageCollector()
: thread(&GarbageCollector::runner, this)
, quitFlag(false)
, nextToClean(0) {
    started = true;
    BL_LOG_INFO << "GarbageCollector online";
}

GarbageCollector::~GarbageCollector() {
    if (!stopped) { stop(); }
}

void GarbageCollector::shutdown() {
    if (started) { get().stop(); }
}

void GarbageCollector::stop() {
    if (thread.joinable()) {
        BL_LOG_INFO << "Terminating GarbageCollector";
        quitFlag = true;
        quitCv.notify_all();
        thread.join();
        stopped = true;
        BL_LOG_INFO << "GarbageCollector terminated";
    }
    else {
        BL_LOG_ERROR << "GarbageCollector already shutdown";
    }
}

GarbageCollector& GarbageCollector::get() {
    static GarbageCollector gc;
    return gc;
}

void GarbageCollector::registerManager(ManagerBase* m) {
    std::unique_lock lock(managerLock);

    // try to find a spot to insert
    for (auto it = managers.begin(); it != managers.end(); ++it) {
        if (it->second > m->gcPeriod) {
            managers.insert(it, MP(m, m->gcPeriod));
            lock.unlock();
            quitCv.notify_all();
            return;
        }
    }

    managers.emplace_back(m, m->gcPeriod);
    lock.unlock();
    quitCv.notify_all();
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

void GarbageCollector::runner() {
    // perform one time setup
    quitFlag = false;
    {
        std::unique_lock lock(managerLock);
        for (auto& mp : managers) { mp.second = mp.first->gcPeriod; }
        std::sort(managers.begin(), managers.end(), Sorter());
        nextToClean = 0;
    }

    while (!quitFlag) {
        std::unique_lock lock(managerLock);
        if (managers.empty()) {
            quitCv.wait_for(lock, std::chrono::seconds(60));
            continue;
        }
        if (quitFlag) return;

        const unsigned int sleptFor = managers[nextToClean].second;
        quitCv.wait_for(lock, std::chrono::seconds(sleptFor));
        if (quitFlag) return;
        if (managers.empty()) continue;

        managers[nextToClean].first->doClean();
        managers[nextToClean].second = managers[nextToClean].first->gcPeriod;

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

        // re-sort now that times have reset
        std::sort(managers.begin(), managers.end(), Sorter());
    }
}

} // namespace resource
} // namespace bl
