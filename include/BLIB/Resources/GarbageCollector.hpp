#ifndef BLIB_RESOURCES_GARBAGECOLLECTOR_HPP
#define BLIB_RESOURCES_GARBAGECOLLECTOR_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace bl
{
namespace resource
{
class ResourceManagerBase;

/**
 * @brief Instantiates the garbage collector and starts resource cleanup in the background
 *
 * @ingroup Resources
 *
 */
class GarbageCollector : private util::NonCopyable {
private:
    using MP = std::pair<ResourceManagerBase*, unsigned int>;
    std::thread thread;
    std::atomic_bool quitFlag;
    std::condition_variable quitCv;
    std::mutex managerLock;
    std::vector<MP> managers;

    GarbageCollector();
    ~GarbageCollector();
    void stop();
    void runner();
    unsigned int soonestIndex() const;

    void registerManager(ResourceManagerBase* manager);
    void unregisterManager(ResourceManagerBase* manager);
    void managerPeriodChanged(ResourceManagerBase* manager);
    friend class ResourceManagerBase;

    // TODO - register bundles as well

    static GarbageCollector& get();
};

} // namespace resource
} // namespace bl

#endif
