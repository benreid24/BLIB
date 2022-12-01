#ifndef BLIB_RESOURCES_FILESYSTEM_HPP
#define BLIB_RESOURCES_FILESYSTEM_HPP

#include <BLIB/Resources/Bundling/BundleRuntime.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <optional>
#include <string>
#include <vector>

namespace bl
{
namespace resource
{
/**
 * @brief File system wrapper which reads data from the underlying filesystem or from the configured
 *        bundle set. Used internally by resource managers
 *
 * @ingroup Resources
 *
 */
class FileSystem : private util::NonCopyable {
public:
    /**
     * @brief Configure the bundle set to use
     *
     * @param bundlePath Path to the directory containing the bundles
     * @return True if the bundle set is valid, false on error
     */
    static bool useBundle(const std::string& bundlePath);

    /**
     * @brief Loads the data for the given path. Either loads from the underlying file system or
     *        from the active bundle
     *
     * @param path The resource path to load
     * @param output The raw data of the given resource
     * @return True if the resource was found and loaded, false otherwise
     */
    static bool getData(const std::string& path, std::vector<char>& output);

    /**
     * @brief Returns whether or not the given path points to a valid resource. Essentially a
     *        bundle-aware version of FileUtil::exists
     *
     * @param path The path to check
     * @return True if there is a resource, false otherwise
     */
    static bool resourceExists(const std::string& path);
};

} // namespace resource
} // namespace bl

#endif
