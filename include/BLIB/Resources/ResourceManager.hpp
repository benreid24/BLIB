#ifndef BLIB_RESOURCES_RESOURCEMANAGER_HPP
#define BLIB_RESOURCES_RESOURCEMANAGER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Resources/ResourceLoader.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace bl
{
/**
 * @brief Single template based resource management class. Meant for storing
 * @brief data exactly once, such as images
 *
 * @ingroup Resources
 */
template<typename TResourceType>
class ResourceManager : private NonCopyable {
public:
    /**
     * @brief Creates a ResourceManager for a given resource type and garbage collection period
     *
     * @param loader ResourceLoader to use. Must remain valid for the lifetime of the manager
     * @param gcPeriod Number of seconds between round of freeing memory
     */
    ResourceManager(ResourceLoader<TResourceType>& loader, unsigned int gcPeriod = 300);

    /**
     * @brief Exits the garbage collection thread. Resources still held are not freed
     *
     */
    ~ResourceManager();

    /**
     * @brief Add an already loaded resource to the manager. Overwrites existing if present
     *
     * @param uri The identifier of the resource
     * @param data The resource to add
     */
    void add(const std::string& uri, typename Resource<TResourceType>::Ref data);

    /**
     * @brief Attempts to find the given resource and return it, loading it if necessary
     *
     * @param uri Some unique string that a ResourceLoader can load the resource with
     * @return The requested resource
     */
    Resource<TResourceType> load(const std::string& uri);

    /**
     * @brief Returns a pointer to the resource for modification (such as forcing it to stay in
     *        cache). Will load the resource if it is not in cache
     *
     * @param uri Some unique string that a ResourceLoader can load the resource with
     * @return Pointer to the requested resource
     */
    Resource<TResourceType>* loadMutable(const std::string& uri);

private:
    ResourceLoader<TResourceType>& loader;
    std::unordered_map<std::string, Resource<TResourceType>> resources;

    const unsigned int gcPeriod;
    std::atomic<bool> gcActive;
    std::thread gcThread;
    std::mutex mapLock;

    void garbageCollector();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ResourceManager<T>::ResourceManager(ResourceLoader<T>& loader, unsigned int gcPeriod)
: loader(loader)
, gcPeriod(gcPeriod)
, gcActive(true)
, gcThread(&ResourceManager<T>::garbageCollector, this) {}

template<typename T>
ResourceManager<T>::~ResourceManager() {
    gcActive = false;
    gcThread.join();
    BL_LOG_INFO << "Resource manager (" << typeid(T).name() << ") shutdown";
}

template<typename T>
void ResourceManager<T>::add(const std::string& uri, typename Resource<T>::Ref resource) {
    mapLock.lock();
    resources[uri] = Resource<T>(resource);
    mapLock.unlock();
}

template<typename T>
Resource<T> ResourceManager<T>::load(const std::string& uri) {
    return *loadMutable(uri);
}

template<typename T>
Resource<T>* ResourceManager<T>::loadMutable(const std::string& uri) {
    auto i = resources.find(uri);
    if (i == resources.end()) {
        mapLock.lock();
        i = resources.insert(std::make_pair(uri, loader.load(uri))).first;
        mapLock.unlock();
    }
    return &i->second;
}

template<typename T>
void ResourceManager<T>::garbageCollector() {
    while (gcActive) {
        for (unsigned int t = 0; t < gcPeriod; ++t) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!gcActive) return;
        }

        mapLock.lock();
        for (auto i = resources.begin(); i != resources.end();) {
            auto j = i++;
            if (j->second.data.unique() && !j->second.forceInCache) resources.erase(j);
        }
        mapLock.unlock();
    }
}

} // namespace bl

#endif