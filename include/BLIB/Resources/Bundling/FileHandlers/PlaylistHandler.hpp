#ifndef BLIB_TEST_TARGET
#ifndef BLIB_RESOURCES_BUNDLING_FILEHANDLERS_PLAYLISTHANDLER_HPP
#define BLIB_RESOURCES_BUNDLING_FILEHANDLERS_PLAYLISTHANDLER_HPP

#include <BLIB/Resources/Bundling/FileHandler.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief File handlers for processing playlists. Loads them as json from disk and saves as binary
 *        in the bundle. Song files are also added to the bundle
 *
 * @ingroup Bundling
 *
 */
struct PlaylistHandler : public FileHandler {
    /**
     * @brief Construct a new Playlist Handler
     *
     * @param bundleMusicFiles True to bundle music with the playlist file, false to leave be
     */
    PlaylistHandler(bool bundleMusicFiles = false);

    virtual ~PlaylistHandler() = default;

    virtual bool processFile(const std::string& path, std::ostream& output,
                             FileHandlerContext& context) override;

    const bool bundleMusicFiles;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif
#endif
