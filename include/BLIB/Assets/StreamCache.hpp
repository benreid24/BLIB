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
     */
    StreamCache() = default;

    // TODO - methods to hook into bundles

    /**
     * @brief Creates and returns a new stream for the given asset and full path
     *
     * @param uuid The asset uuid the stream is being created for
     * @param path The full path of the file to create the stream from
     * @return A pointer to the new stream, or nullptr if the stream could not be created
     */
    PersistentStream* getStream(util::UUID uuid, const std::string& path);

    /**
     * @brief Releases all streams for the given asset. Called when an asset is unloaded
     *
     * @param uuid The asset uuid to release streams for
     */
    void releaseStreams(util::UUID uuid);

private:
    std::unordered_map<util::UUID, std::list<PersistentStream>> streams;
    // TODO - bundle backing store when implemented
};

} // namespace as
} // namespace bl

#endif
