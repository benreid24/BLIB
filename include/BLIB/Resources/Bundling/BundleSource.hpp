#ifndef BLIB_RESOURCES_BUNDLING_BUNDLESOURCE_HPP
#define BLIB_RESOURCES_BUNDLING_BUNDLESOURCE_HPP

#include <string>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Config struct defining a top level starting point for a bundle
 *
 * @ingroup Bundling
 *
 */
struct BundleSource {
    /// @brief The top level file or folder to start the bundle with
    std::string sourcePath;

    /// @brief Determines the behavior when sourcePath is a directory
    enum Policy {
        /// @brief All files are placed in the same bundle
        BundleAllFiles,

        /// @brief Each contained file/folder gets its own bundle
        CreateBundleForEachContained,

        /// @brief Every file gets its own bundle, including files in folders
        CreateBundleForEachContainedRecursive
    } policy;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif
