#ifndef BLIB_RESOURCES_ASYNCRESOURCELOADER_HPP
#define BLIB_RESOURCES_ASYNCRESOURCELOADER_HPP

#include <BLIB/Resources/ResourceManager.hpp>
#include <fstream>

namespace bl
{
/**
 * @brief Specialized loader meant for loading resources into a ResourceManager in the
 *        background. Default implementation simply calls load() on the manager, but
 *        custom background loaders can be built for large resources and should leverage
 *        ResourceManager::add() to insert constructed resources
 *
 * @see BackgroundResourceLoader
 * @ingroup Resources
 */
template<typename TResourceType>
struct AsyncResourceLoader {
    /**
     * @brief Load a resource into the manager. Simply calls manager.load() but custom
     *        loaders may implement their loading and progress reporting here
     *
     * @param manager The resource manager to load into
     * @param uri The resource identifier to load and add
     * @param progress Optionally update the progress (in range [0,1]) as loading is done
     */
    virtual void load(ResourceManager<TResourceType>& manager, const std::string& uri,
                      float& progress);

    /**
     * @brief Estimate the relative amount of work required for the given resource. The number
     *        itself is arbitary and the value does not matter. It must simply be on the same
     *        scale as the other numbers returned by this method for different resources. The
     *        default behavior returns the file size, in bytes, divided by 10,000
     *
     * @param uri The resource identifier to estimate the work load for
     * @return float Relative estimate of the work required to load it
     */
    virtual float estimateWork(const std::string& uri);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void AsyncResourceLoader<T>::load(ResourceManager<T>& manager, const std::string& uri,
                                  float& progress) {
    manager.load(uri);
    progress = 1;
}

template<typename T>
float AsyncResourceLoader<T>::estimateWork(const std::string& uri) {
    std::ifstream in(uri, std::ifstream::ate | std::ifstream::binary);
    return static_cast<float>(in.tellg()) / 10000.f;
}

} // namespace bl

#endif