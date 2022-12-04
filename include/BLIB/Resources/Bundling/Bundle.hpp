#ifndef BLIB_RESOURCES_BUNDLING_BUNDLE_HPP
#define BLIB_RESOURCES_BUNDLING_BUNDLE_HPP

#include <BLIB/Resources/Bundling/BundleMetadata.hpp>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Bundle to be used at runtime by BundleRuntime. Do not use directly
 *
 * @ingroup Bundling
 *
 */
class Bundle {
public:
    /// @brief Bundles expire after 60 seconds of no interaction
    static constexpr unsigned int Lifetime = 60;

    /**
     * @brief Loads the bundle from the path
     *
     * @param path The bundle file to load
     */
    Bundle(const std::string& path);

    /**
     * @brief Loads the given resource from the bundle
     *
     * @param path The resource path to load
     * @param buffer Pointer to buffer to point to the resource data
     * @param len Length variable to populate with resource data length
     * @return True if the resource could be loaded, false otherwise
     */
    bool getResource(const std::string& path, char** buffer, std::size_t& len);

    /**
     * @brief Returns whether or not this bundle has expired and should be purged
     *
     * @return True if the bundle should be cleaned up, false otherwise
     */
    bool expired() const;

private:
    const std::string source;
    BundleMetadata manifest;
    std::chrono::steady_clock::time_point touchTime;
    std::vector<char> data;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif