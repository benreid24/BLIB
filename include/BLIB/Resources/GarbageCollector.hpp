#ifndef BLIB_RESOURCES_GARBAGECOLLECTOR_HPP
#define BLIB_RESOURCES_GARBAGECOLLECTOR_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

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
class GarbageCollector {
public:
    /**
     * @brief Starts the garbage collector
     *
     */
    GarbageCollector();

    /**
     * @brief Terminates the garbage collector
     *
     */
    ~GarbageCollector();

private:
    static void registerManager(ManagerBase* manager);
    static void unregisterManager(ManagerBase* manager);
    friend class ManagerBase;
};
} // namespace resource
} // namespace bl

#endif
