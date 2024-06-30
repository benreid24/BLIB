#ifndef BLIB_RESOURCES_RESOURCEMANAGER_HPP
#define BLIB_RESOURCES_RESOURCEMANAGER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Resources/FileSystem.hpp>
#include <BLIB/Resources/Loader.hpp>
#include <BLIB/Resources/Ref.hpp>
#include <BLIB/Util/BufferIstreamBuf.hpp>
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
 */
class ResourceManagerBase {
protected:
    ResourceManagerBase();
    void unregister();
    void setGCPeriod(unsigned int period);

private:
    virtual void doClean() = 0;
    unsigned int gcPeriod;

    virtual void freeAll() = 0;

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
     * @brief Query whether the resource with the given URI is available in the resource manager
     *
     * @param uri The URI to check
     * @return True if the resource is loaded, false otherwise
     */
    static bool available(const std::string& uri);

    /**
     * @brief Returns a pointer to the resource for modification (such as forcing it to stay in
     *        cache). Will load the resource if it is not in cache
     *
     * @param uri Some unique string that a Loader can load the resource with
     * @return Pointer to the requested resource
     */
    static Ref<TResourceType> load(const std::string& uri);

    /**
     * @brief Similar to load() but bypasses the filesystem and directly calls the loader with the
     *        URI. Intended for generated/composite resources where the URI is not a file path and
     *        the loader creates the resource from other sources. Only use when a compatible loader
     *        is set as the data pointers passed to it will always be nullptr
     *
     * @param uri The resource path to pass to the loader
     * @return A ref to the requested resource
     */
    static Ref<TResourceType> getOrCreateGenerated(const std::string& uri);

    /**
     * @brief Uses the underlying FileSystem and loader to initialize the existing resource. The
     *        resource will not be managed. This is useful when dynamic management is not necessary
     *        but you still want the resource loading to be bundle and loader aware
     *
     * @param uri The resource path to initialize from
     * @param resource The resource to initialize
     * @return True if the resource could be initialized, false otherwise
     */
    static bool initializeExisting(const std::string& uri, TResourceType& resource);

    /**
     * @brief Manually insert a resource into the manager. Will be managed as normal
     *
     * @param uri The uri of the resource
     * @param resource The resource to insert
     * @return A handle to the inserted resource
     */
    static Ref<TResourceType> put(const std::string& uri, TResourceType& resource);

    /**
     * @brief Explicitly frees and destroys all resources, regardless of ownership state. Be very
     *        careful with this
     */
    static void freeAndDestroyAll();

private:
    std::unique_ptr<LoaderBase<TResourceType>> loader;
    std::unordered_map<std::string, Resource<TResourceType>> resources;
    std::mutex mapLock;

    static ResourceManager& get();

    ResourceManager();
    virtual void doClean() override;
    bool doInit(const std::string& uri, char* buf, std::size_t len, TResourceType& result);
    virtual void freeAll() override;
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
    get().setGCPeriod(gc);
}

template<typename TResourceType>
bool ResourceManager<TResourceType>::available(const std::string& uri) {
    ResourceManager& m = get();
    std::unique_lock lock(m.mapLock);
    const auto it = m.resources.find(uri);
    return it != m.resources.end();
}

template<typename T>
Ref<T> ResourceManager<T>::load(const std::string& uri) {
    ResourceManager& m = get();
    std::unique_lock lock(m.mapLock);
    auto it = m.resources.find(uri);
    if (it == m.resources.end()) {
        it = m.resources.try_emplace(uri).first;

        char* buffer    = nullptr;
        std::size_t len = 0;
        if (!FileSystem::getData(uri, &buffer, len)) {
            BL_LOG_ERROR << "Failed to find resource: " << uri;
            return {&it->second};
        }

        // copy buffer for font
        if constexpr (std::is_same_v<T, sf::Font>) {
            std::vector<char>& rbuf = it->second.buffer;
            rbuf.resize(len);
            std::memcpy(rbuf.data(), buffer, len);
            buffer = rbuf.data();
        }

        if (!m.doInit(uri, buffer, len, it->second.data)) {
            BL_LOG_ERROR << "Failed to load resource: " << uri;
        }
    }

    return {&it->second};
}

template<typename TResourceType>
Ref<TResourceType> ResourceManager<TResourceType>::getOrCreateGenerated(const std::string& uri) {
    ResourceManager& m = get();
    std::unique_lock lock(m.mapLock);
    auto it = m.resources.find(uri);
    if (it == m.resources.end()) {
        it = m.resources.try_emplace(uri).first;

        util::BufferIstreamBuf buf(nullptr, 0);
        std::istream stream(&buf);
        if (!m.loader->load(uri, nullptr, 0, stream, it->second.data)) {
            BL_LOG_ERROR << "Failed to create resource: " << uri;
        }
    }

    return {&it->second};
}

template<typename T>
bool ResourceManager<T>::initializeExisting(const std::string& uri, T& result) {
    ResourceManager& m = get();

    char* buffer    = nullptr;
    std::size_t len = 0;
    if (!FileSystem::getData(uri, &buffer, len)) {
        BL_LOG_ERROR << "Failed to find resource: " << uri;
        return false;
    }
    return m.doInit(uri, buffer, len, result);
}

template<typename TResourceType>
Ref<TResourceType> ResourceManager<TResourceType>::put(const std::string& uri,
                                                       TResourceType& resource) {
    ResourceManager& m = get();
    std::unique_lock lock(m.mapLock);
    auto it = m.resources.find(uri);
    if (it == m.resources.end()) {
        it              = m.resources.try_emplace(uri).first;
        it->second.data = resource;
    }
    return {&it->second};
}

template<typename T>
bool ResourceManager<T>::doInit(const std::string& uri, char* buffer, std::size_t len, T& result) {
    util::BufferIstreamBuf buf(buffer, len);
    std::istream stream(&buf);
    const bool success = loader->load(uri, buffer, len, stream, result);
    FileSystem::purgePersistentData(uri);

    return success;
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
        if (j->second.readyForPurge()) {
            BL_LOG_DEBUG << "Purged expired resource: " << j->first;
            resources.erase(j);
        }
    }
}

template<typename T>
void ResourceManager<T>::freeAll() {
    std::unique_lock lock(mapLock);
    resources.clear();
}

template<typename T>
ResourceManager<T>& ResourceManager<T>::get() {
    static ResourceManager me;
    return me;
}

template<typename T>
void ResourceManager<T>::freeAndDestroyAll() {
    get().freeAll();
}

} // namespace resource
} // namespace bl

#endif
