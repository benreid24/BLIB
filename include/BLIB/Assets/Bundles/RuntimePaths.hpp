#ifndef BLIB_ASSETS_BUNDLES_RUNTIMEPATHS_HPP
#define BLIB_ASSETS_BUNDLES_RUNTIMEPATHS_HPP

#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/UUID.hpp>
#include <string>
#include <string_view>

namespace bl
{
namespace as
{
namespace bdl
{
/**
 * @brief Collection of paths and path helpers for the bundle runtime
 *
 * @ingroup Assets
 */
struct RuntimePaths {
    static constexpr std::string_view ManifestPath = "manifest.mst";
    static constexpr const char* BundleExtention   = ".bdl";

    /**
     * @brief Returns the path to the bundle file for the given bundle
     *
     * @param base The base path containing bundles and the manifest
     * @param uuid The UUID of the bundle to get the path for
     * @return The full path to the bundle file
     */
    static std::string getBundlePath(const std::string& base, util::UUID uuid) {
        return util::FileUtil::joinPath(base, uuid.toString()) + BundleExtention;
    }
};

} // namespace bdl
} // namespace as
} // namespace bl

#endif
