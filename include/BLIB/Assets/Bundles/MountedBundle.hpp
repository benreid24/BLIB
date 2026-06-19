#ifndef BLIB_ASSETS_BUNDLES_MOUNTEDBUNDLE_HPP
#define BLIB_ASSETS_BUNDLES_MOUNTEDBUNDLE_HPP

#include <BLIB/Assets/Bundles/BundleData.hpp>
#include <BLIB/Streams/InputStream.hpp>
#include <chrono>
#include <string_view>

namespace bl
{
namespace as
{
class Repository;

namespace bdl
{
class BundleRuntime;

/**
 * @brief Interface for a mounted bundle file of asset data
 *
 * @ingroup Assets
 */
class MountedBundle {
public:
    /**
     * @brief Initializes the mounted bundle
     *
     * @param repo The owning asset repository
     * @param path The path to the bundle file
     */
    MountedBundle(Repository& repo, const std::string& path);

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
     * @brief Returns whether the bundle is expired
     */
    bool isExpired() const;

private:
    std::string bundlePath;
    BundleData data;
    std::chrono::steady_clock::time_point touchTime;

    const FileMetadata* findFile(util::UUID uuid, std::string_view path);

    friend class BundleRuntime;
};

} // namespace bdl
} // namespace as
} // namespace bl

#endif
