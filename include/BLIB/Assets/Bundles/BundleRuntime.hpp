#ifndef BLIB_ASSETS_BUNDLES_BUNDLERUNTIME_HPP
#define BLIB_ASSETS_BUNDLES_BUNDLERUNTIME_HPP

#include <BLIB/Assets/Bundles/Manifest.hpp>
#include <BLIB/Assets/Bundles/MountedBundle.hpp>
#include <mutex>

namespace bl
{
namespace as
{
class Repository;

namespace bdl
{
/**
 * @brief Runtime manager for bundles. Used by the asset manager
 *
 * @ingroup Assets
 */
class BundleRuntime {
public:
    /**
     * @brief Initializes the bundle runtime
     *
     * @param repo The owning repository
     * @param bundlePath The path to the directory containing the manifest and bundles
     */
    BundleRuntime(Repository& repo, const std::string& bundlePath);

    /**
     * @brief Initializes an input stream from the in-memory bundle data. Stream will be invalidated
     *        when the bundle is ejected
     *
     * @param stream The stream to initialize
     * @param assetId The UUID of the asset requesting data
     * @param localPath The asset local file path
     * @return True if the stream could be initialized, false otherwise
     */
    bool initStream(stream::InputStream& stream, util::UUID assetId, std::string_view localPath);

    /**
     * @brief Initializes an input stream directly from the bundle on disk. Stream remains valid for
     *        the program lifetime. Use for persistent streams
     *
     * @param stream The stream to initialize
     * @param assetId The UUID of the asset requesting data
     * @param localPath The asset local file path
     * @return True if the stream could be initialized, false otherwise
     */
    bool initStreamDirect(stream::InputStream& stream, util::UUID assetId,
                          std::string_view localPath);

    /**
     * @brief Releases stale bundles
     */
    void releaseStale();

private:
    std::recursive_mutex mutex;
    Repository& repo;
    std::string path;
    Manifest manifest;
    std::unordered_map<util::UUID, MountedBundle> mountedBundles;

    MountedBundle* getBundle(util::UUID uuid);

    friend class ::bl::as::Repository;
};

} // namespace bdl
} // namespace as
} // namespace bl

#endif
