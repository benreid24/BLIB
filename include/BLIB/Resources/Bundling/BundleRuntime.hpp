#ifndef BLIB_RESOURCES_BUNDLING_BUNDLERUNTIME_HPP
#define BLIB_RESOURCES_BUNDLING_BUNDLERUNTIME_HPP

#include <BLIB/Resources/Bundling/Bundle.hpp>
#include <BLIB/Resources/Bundling/Manifest.hpp>
#include <mutex>
#include <unordered_map>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Runtime for bundles used by FileSystem. Do not use directly
 *
 * @ingroup Bundling
 *
 */
class BundleRuntime {
public:
    /**
     * @brief Construct a new Bundle Runtime in an empty state
     *
     * @param bundlePath The path where the bundles and manifest are
     */
    BundleRuntime(const std::string& bundlePath);

    /**
     * @brief Load the given bundle path
     *
     * @return True if the manifest could be loaded, false otherwise
     */
    bool initialize();

    /**
     * @brief Fetch the given resource from the bundles
     *
     * @param path The resource path to get
     * @param buffer The buffer to populate
     * @param len The length to populate
     * @return True if the resource was loaded, false otherwise
     */
    bool getResource(const std::string& path, char** buffer, std::size_t& len);

    /**
     * @brief Returns whether or not a resource with the given path exists
     *
     * @param path The resource path to check
     * @return True if the resource is in the bundles, false otherwise
     */
    bool resourceExists(const std::string& path) const;

    /**
     * @brief Purges expired bundles
     *
     */
    void clean();

private:
    std::mutex cleanMutex;
    Manifest manifest;
    std::unordered_map<std::string, Bundle> bundles;

    Bundle* getOrLoadBundle(const std::string& resourcePath);
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif