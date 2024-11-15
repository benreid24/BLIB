#ifndef BLIB_RESOURCES_GARBAGECOLLECTOR_HPP
#define BLIB_RESOURCES_GARBAGECOLLECTOR_HPP

#include <BLIB/Resources/Bundling/BundleRuntime.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

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
public:
    /**
     * @brief Shuts down the GC if running and frees all remaining resources. Call at the end of
     *        main()
     */
    static void shutdownAndClear();

private:
    using MP = std::pair<ResourceManagerBase*, unsigned int>;
    std::thread thread;
    std::atomic_bool quitFlag;
    std::condition_variable quitCv;
    std::mutex managerLock;
    std::vector<MP> managers;
    bundle::BundleRuntime* bundleRuntime;

    GarbageCollector();
    ~GarbageCollector();
    void stop();
    void clear();
    void runner();
    unsigned int soonestIndex() const;

    void registerManager(ResourceManagerBase* manager);
    void unregisterManager(ResourceManagerBase* manager);
    void managerPeriodChanged(ResourceManagerBase* manager);
    friend class ResourceManagerBase;

    void registerBundleRuntime(bundle::BundleRuntime* runtime);
    friend class FileSystem;

    static GarbageCollector& get();

    friend class engine::Engine;
};

} // namespace resource
} // namespace bl

#endif
