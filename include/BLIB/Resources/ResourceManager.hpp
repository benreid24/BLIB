#ifndef BLIB_RESOURCES_RESOURCEMANAGER_HPP
#define BLIB_RESOURCES_RESOURCEMANAGER_HPP

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
template<typename T, class TLoader>
class ResourceManager : private NonCopyable {
public:
    typedef typename Resource<T>::Ref RefType;

    /**
     * @brief Creates a ResourceManager for a given resource type and garbage collection period
     *
     * @param gcPeriod Number of seconds between round of freeing memory
     */
    ResourceManager(unsigned int gcPeriod = 300);
    ~ResourceManager();

    /**
     * @brief Attempts to find the given resource and return it, loading it if necessary
     *
     * @param uri Some unique string that a ResourceLoader can load the resource with
     */
    typename Resource<T>::Ref load(const std::string& uri);

private:
    TLoader loader;
    std::unordered_map<std::string, typename Resource<T>::Ref> resources;

    const unsigned int gcPeriod;
    std::atomic<bool> gcActive;
    std::thread gcThread;
    std::mutex mapLock;

    void garbageCollector();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, class TLoader>
ResourceManager<T, TLoader>::ResourceManager(unsigned int gcPeriod)
: gcPeriod(gcPeriod)
, gcActive(true)
, gcThread(&ResourceManager<T, TLoader>::garbageCollector, this) {}

template<typename T, class TLoader>
ResourceManager<T, TLoader>::~ResourceManager() {
    gcActive = false;
    gcThread.join();
}

template<typename T, class TLoader>
typename ResourceManager<T, TLoader>::RefType ResourceManager<T, TLoader>::load(
    const std::string& uri) {
    auto i = resources.find(uri);
    if (i == resources.end()) {
        mapLock.lock();
        i = resources.insert(std::make_pair(uri, loader(uri))).first;
        mapLock.unlock();
    }
    return i->second;
}

template<typename T, class TLoader>
void ResourceManager<T, TLoader>::garbageCollector() {
    while (gcActive) {
        for (unsigned int t = 0; t < gcPeriod; ++t) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!gcActive) return;
        }

        mapLock.lock();
        for (auto i = resources.begin(); i != resources.end();) {
            auto j = i++;
            if (j->second.unique()) resources.erase(j);
        }
        mapLock.unlock();
    }
}

/// Specialized ResourceManager for sf::Texture objects
typedef ResourceManager<sf::Texture, TextureResourceLoader> TextureResourceManager;

// Specialized ResourceManager for sf::Font objects
typedef ResourceManager<sf::Font, FontResourceLoader> FontResourceManager;

} // namespace bl

#endif