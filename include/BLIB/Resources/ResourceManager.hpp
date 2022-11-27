#ifndef BLIB_RESOURCES_RESOURCEMANAGER_HPP
#define BLIB_RESOURCES_RESOURCEMANAGER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Resources/FileSystem.hpp>
#include <BLIB/Resources/Loader.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace bl
{
/// Collection of classes for resource management and flyweight pattern
namespace resource
{
class GarbageCollector;

/**
 * @brief Base class for resource managers, do not use
 *
 * @ingroup Resources
 *
 */
class ResourceManagerBase {
protected:
    ResourceManagerBase();
    void unregister();
    void getGCPeriod(unsigned int period);

private:
    virtual void doClean() = 0;
    unsigned int gcPeriod;

    friend class GarbageCollector;
};

/**
 * @brief Single template based resource management class. Meant for storing
 *        data exactly once, such as images
 *
 * @ingroup Resources
 */
template<typename TResourceType>
class ResourceManager
: private util::NonCopyable
, public ResourceManagerBase {
public:
    /**
     * @brief Exits the garbage collection thread. Resources still held are not freed
     *
     */
    ~ResourceManager();

    /**
     * @brief Configures the resource manager with the given loader. Call this before loading any
     *        resources
     *
     * @tparam TLoader The type of loader to create. Must derive from Loader
     * @tparam TArgs Argument types to forward to loader constructor
     * @param loaderArgs The arguments to forward to the loader constructor
     */
    template<typename TLoader, typename... TArgs>
    static void installLoader(TArgs&&... loaderArgs);

    /**
     * @brief Set the time, in seconds, between resource cleanup runs
     *
     * @param period Time between runs will never be less than this
     */
    static void setGarbageCollectionPeriod(unsigned int period);

    /**
     * @brief Returns a pointer to the resource for modification (such as forcing it to stay in
     *        cache). Will load the resource if it is not in cache
     *
     * @param uri Some unique string that a Loader can load the resource with
     * @return Pointer to the requested resource
     */
    static Resource<TResourceType>& load(const std::string& uri);

private:
    std::unique_ptr<LoaderBase<TResourceType>> loader;
    std::unordered_map<std::string, Resource<TResourceType>> resources;
    std::mutex mapLock;

    static ResourceManager& get();

    ResourceManager();
    virtual void doClean() override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ResourceManager<T>::ResourceManager()
: ResourceManagerBase()
, loader(std::make_unique<DefaultLoader<T>>()) {
    BL_LOG_INFO << "Resource manager (" << typeid(T).name() << ") online";
}

template<typename T>
ResourceManager<T>::~ResourceManager() {
    unregister();
    BL_LOG_INFO << "Resource manager (" << typeid(T).name() << ") shutdown";
}

template<typename T>
void ResourceManager<T>::setGarbageCollectionPeriod(unsigned int gc) {
    get().getGCPeriod(gc);
}

template<typename T>
Resource<T>& ResourceManager<T>::load(const std::string& uri) {
    ResourceManager& m = get();
    std::unique_lock lock(m.mapLock);
    auto it = m.resources.find(uri);
    if (it == m.resources.end()) {
        std::vector<char> data;
        if (!FileSystem::getData(uri, data)) { BL_LOG_ERROR << "Failed to load resource: " << uri; }
        it = m.resources.insert(std::make_pair(uri, m.loader->load(uri, data))).first;
    }
    return it->second;
}

template<typename T>
template<typename TLoader, typename... TArgs>
void ResourceManager<T>::installLoader(TArgs&&... args) {
    get().loader = std::make_unique<TLoader>(std::forward<TArgs>(args)...);
}

template<typename T>
void ResourceManager<T>::doClean() {
    std::unique_lock lock(mapLock);
    for (auto i = resources.begin(); i != resources.end();) {
        auto j = i++;
        if (j->second.data.unique() && !j->second.forceInCache) resources.erase(j);
    }
}

template<typename T>
ResourceManager<T>& ResourceManager<T>::get() {
    static ResourceManager me;
    return me;
}

} // namespace resource
} // namespace bl

#endif