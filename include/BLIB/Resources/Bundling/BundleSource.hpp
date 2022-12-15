#ifndef BLIB_RESOURCES_BUNDLING_BUNDLESOURCE_HPP
#define BLIB_RESOURCES_BUNDLING_BUNDLESOURCE_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <regex>
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

    /// @brief Regex pattern to whitelist files to include. Leave empty for no whitelist
    std::string whitelistPattern;

    /// @brief Regex pattern to whitelist files to exclude. Leave empty for no blacklist
    std::string blacklistPattern;

    /**
     * @brief Construct a new Bundle Source from the given parameters
     *
     * @param srcPath The path to search or a single file to include
     * @param policy The policy for creating bundles from the source path
     * @param whitelistPattern The pattern to whitelist files with. Empty for no whitelist
     * @param blacklistPattern The pattern to blacklist files with. Empty for no blacklist
     */
    BundleSource(const std::string& srcPath, Policy policy,
                 const std::string& whitelistPattern = "", const std::string& blacklistPattern = "")
    : sourcePath(srcPath)
    , policy(policy)
    , whitelistPattern(whitelistPattern)
    , blacklistPattern(blacklistPattern)
    , whitelist(whitelistPattern)
    , blacklist(blacklistPattern) {}

    /**
     * @brief Returns whether or not the given file within this bundle source should be bundled
     *        based on the whitelist and blacklist patterns
     *
     * @param file The file to check
     * @return True if the file should be bundled, false otherwise
     */
    bool includeFile(const std::string& file) const {
        if (!whitelistPattern.empty() && !std::regex_match(file.c_str(), whitelist)) {
            BL_LOG_DEBUG << "Excluding file '" << file
                         << "' due to bundle whitelist pattern: " << whitelistPattern;
            return false;
        }

        if (!blacklistPattern.empty() && std::regex_match(file.c_str(), blacklist)) {
            BL_LOG_DEBUG << "Excluding file '" << file
                         << "' due to bundle blacklist pattern: " << blacklistPattern;
            return false;
        }

        return true;
    }

private:
    std::regex whitelist;
    std::regex blacklist;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif
