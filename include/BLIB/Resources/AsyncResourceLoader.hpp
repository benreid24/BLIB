#ifndef BLIB_RESOURCES_ASYNCRESOURCELOADER_HPP
#define BLIB_RESOURCES_ASYNCRESOURCELOADER_HPP

#include <BLIB/Resources/ResourceManager.hpp>

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
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void AsyncResourceLoader<T>::load(ResourceManager<T>& manager, const std::string& uri,
                                  float& progress) {
    manager.load(uri);
    progress = 1;
}

} // namespace bl

#endif