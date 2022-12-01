#include <BLIB/Media/Audio/Playlist.hpp>
#include <BLIB/Resources/Bundling/FileHandlers/PlaylistHandler.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
PlaylistHandler::PlaylistHandler(bool bdeps)
: bundleMusicFiles(bdeps) {}

bool PlaylistHandler::processFile(const std::string& path, std::ostream& output,
                                  FileHandlerContext& ctx) {
    audio::Playlist playlist;
    if (!playlist.loadFromFile(path)) return false;

    serial::StreamOutputBuffer wrapper(output);
    serial::binary::OutputStream os(wrapper);
    if (!playlist.saveToMemory(os)) return false;

    if (bundleMusicFiles) {
        for (const std::string& song : playlist.getSongList()) { ctx.addDependencyFile(song); }
    }
    return true;
}

} // namespace bundle
} // namespace resource
} // namespace bl
