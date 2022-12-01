#include <BLIB/Resources/GarbageCollector.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Resources/ResourceManager.hpp>
#include <optional>
#include <vector>

namespace bl
{
namespace resource
{
namespace
{
std::atomic_bool started = false;
std::atomic_bool stopped = false;
} // namespace

GarbageCollector::GarbageCollector()
: thread(&GarbageCollector::runner, this)
, quitFlag(false) {
    started = true;
    BL_LOG_INFO << "GarbageCollector online";
}

GarbageCollector::~GarbageCollector() {
    if (!stopped) { stop(); }
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

void GarbageCollector::registerManager(ResourceManagerBase* m) {
    std::unique_lock lock(managerLock);
    managers.emplace_back(m, m->gcPeriod);
    lock.unlock();
    quitCv.notify_all();
}

void GarbageCollector::managerPeriodChanged(ResourceManagerBase* m) {
    std::unique_lock lock(managerLock);
    for (auto& rm : managers) {
        if (rm.first == m) { rm.second = m->gcPeriod; }
    }
    lock.unlock();
    quitCv.notify_all();
}

void GarbageCollector::unregisterManager(ResourceManagerBase* m) {
    std::unique_lock lock(managerLock);

    for (auto it = managers.begin(); it != managers.end(); ++it) {
        if (it->first == m) {
            managers.erase(it);
            break;
        }
    }
}

void GarbageCollector::registerBundleRuntime(bundle::BundleRuntime* br) {
    std::unique_lock lock(managerLock);
    bundleRuntime = br;
}

void GarbageCollector::runner() {
    while (!quitFlag) {
        std::unique_lock lock(managerLock);

        // purge expired bundles
        if (bundleRuntime) { bundleRuntime->clean(); }

        // block if no managers to clean
        if (managers.empty()) {
            quitCv.wait_for(lock, std::chrono::seconds(60));
            continue;
        }

        // determine the next time we need to clean and sleep until then
        auto& mp                     = managers[soonestIndex()];
        const unsigned int sleepTime = mp.second;
        const auto startTime         = std::chrono::steady_clock::now();
        quitCv.wait_for(lock, std::chrono::seconds(sleepTime));
        const unsigned int sleptTime = std::chrono::duration_cast<std::chrono::seconds>(
                                           std::chrono::steady_clock::now() - startTime)
                                           .count();

        // check if we need to bail
        if (quitFlag) return;
        if (managers.empty()) continue;

        // proceed through and clean managers that need it
        for (auto& omp : managers) {
            if (quitFlag) return;

            if (omp.second <= sleptTime) {
                omp.first->doClean();
                omp.second = omp.first->gcPeriod;
            }
            else {
                omp.second -= sleptTime;
            }
        }
    }
}

unsigned int GarbageCollector::soonestIndex() const {
    unsigned int nt = 1000000;
    unsigned int mi = 0;
    for (unsigned int i = 0; i < managers.size(); ++i) {
        if (managers[i].second < nt) {
            nt = managers[i].second;
            mi = i;
        }
    }
    return mi;
}

} // namespace resource
} // namespace bl
