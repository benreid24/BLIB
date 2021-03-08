#ifndef BLIB_RESOURCES_BACKGROUNDRESOURCELOADER_HPP
#define BLIB_RESOURCES_BACKGROUNDRESOURCELOADER_HPP

#include <BLIB/Resources/AsyncLoader.hpp>

#include <SFML/System.hpp>
#include <functional>
#include <vector>

namespace bl
{
namespace resource
{
/**
 * @brief Utility class for loading resources in the background while monitoring the progress.
 *        Resource loading is done from the calling thread, and monitoring is done on a second
 *        thread. This is to eliminate any context issues with loading SFML resources
 *
 * @ingroup Resources
 */
template<typename TResourceType>
class BackgroundLoader {
public:
    /// Progress report callback. Parameter is completion in range [0,1]
    typedef std::function<void(float)> ProgressCallback;

    /**
     * @brief Create a new BackgroundLoader
     *
     * @param manager The resource manager to populate
     * @param loader The background loader to use
     * @param callback The progress reporting callback to call from the second thread
     * @param cbPeriod Time, in seconds, in between calls to the callback
     */
    BackgroundLoader(Manager<TResourceType>& manager, AsyncLoader<TResourceType>& loader,
                     ProgressCallback callback, float cbPeriod = 0.01f);

    /**
     * @brief Add a new uri to be loaded
     *
     * @param uri The resource identifier
     * @param estimate Estimated work. Arbitrary value, relative to others passed in
     */
    void addResourceToQueue(const std::string& uri, float estimate);

    /**
     * @brief Add a new uri to be loaded and use the AsyncLoader to determine the
     *        estimate
     *
     * @param uri The resource identifier
     */
    void addResourceToQueue(const std::string& uri);

    /**
     * @brief Performs the actual loading. Starts the monitoring thread and does not return
     *        until all loading is complete
     *
     */
    void load();

private:
    std::atomic<bool> loadingActive;
    std::thread monitorThread;

    Manager<TResourceType>& manager;
    AsyncLoader<TResourceType>& loader;
    const ProgressCallback callback;
    const float callbackPeriod;

    std::vector<std::pair<std::string, float>> uris;
    float totalWork;
    float inProgress;
    float currentItem;
    float completed;

    void monitor();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
BackgroundLoader<T>::BackgroundLoader(Manager<T>& manager, AsyncLoader<T>& loader,
                                      ProgressCallback callback, float cbPeriod)
: loadingActive(false)
, manager(manager)
, loader(loader)
, callback(callback)
, callbackPeriod(cbPeriod)
, totalWork(0)
, inProgress(0)
, currentItem(0)
, completed(0) {}

template<typename T>
void BackgroundLoader<T>::addResourceToQueue(const std::string& uri, float estimate) {
    uris.push_back(std::make_pair(uri, estimate));
    totalWork += estimate;
}

template<typename T>
void BackgroundLoader<T>::addResourceToQueue(const std::string& uri) {
    const float estimate = loader.estimateWork(uri);
    uris.push_back(std::make_pair(uri, estimate));
    totalWork += estimate;
}

template<typename T>
void BackgroundLoader<T>::load() {
    loadingActive = true;
    monitorThread = std::thread(&BackgroundLoader<T>::monitor, this);

    for (const auto& item : uris) {
        inProgress  = 0;
        currentItem = item.second;
        loader.load(manager, item.first, inProgress);
        completed += currentItem;
        currentItem = 0;
    }

    loadingActive = false;
    monitorThread.join();
}

template<typename T>
void BackgroundLoader<T>::monitor() {
    const int usSleep = callbackPeriod * 1000000;
    while (loadingActive) {
        std::this_thread::sleep_for(std::chrono::microseconds(usSleep));
        callback((completed + inProgress * currentItem) / totalWork);
    }
}

} // namespace resource
} // namespace bl

#endif
