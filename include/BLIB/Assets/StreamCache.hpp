#ifndef BLIB_ASSETS_STREAMCACHE_HPP
#define BLIB_ASSETS_STREAMCACHE_HPP

#include <BLIB/Assets/PersistentStream.hpp>
#include <BLIB/Util/UUID.hpp>
#include <list>
#include <unordered_map>

namespace bl
{
namespace as
{
class Repository;
class Asset;

namespace bdl
{
class BundleRuntime;
}

/**
 * @brief Utility that provides persistent streams for asset payloads that require continued access
 *        to their data such as sf::Music
 *
 * @ingroup Assets
 */
class StreamCache {
public:
    /**
     * @brief Creates an empty stream cache
     *
     * @param repoRoot The root path of the asset repository
     */
    StreamCache(const std::string& repoRoot);

    /**
     * @brief Uses the provided bundle runtime instead of direct file access
     *
     * @param runtime The bundle runtime to use
     */
    void useBundles(bdl::BundleRuntime* runtime);

    /**
     * @brief Creates and returns a new stream for the given asset and full path
     *
     * @param uuid The asset uuid the stream is being created for
     * @param localPath The asset local path to get the stream for
     * @return A pointer to the new stream, or nullptr if the stream could not be created
     */
    PersistentStream* getStream(Asset& asset, std::string_view localPath);

    /**
     * @brief Releases all streams for the given asset. Called when an asset is unloaded
     *
     * @param uuid The asset uuid to release streams for
     */
    void releaseStreams(util::UUID uuid);

private:
    const std::string& repoRoot;
    std::unordered_map<util::UUID, std::list<PersistentStream>> streams;
    bdl::BundleRuntime* runtime;
};

} // namespace as
} // namespace bl

#endif
