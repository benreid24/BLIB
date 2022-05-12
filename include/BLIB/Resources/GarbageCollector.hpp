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
class ManagerBase;

/**
 * @brief Instantiates the garbage collector and starts resource cleanup in the background
 *
 * @ingroup Resources
 *
 */
class GarbageCollector : private util::NonCopyable {
public:
    static void shutdown();

private:
    using MP = std::pair<ManagerBase*, unsigned int>;
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

    void registerManager(ManagerBase* manager);
    void unregisterManager(ManagerBase* manager);
    friend class ManagerBase;

    static GarbageCollector& get();
};
} // namespace resource
} // namespace bl

#endif
