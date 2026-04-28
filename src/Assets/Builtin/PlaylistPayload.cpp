#include <BLIB/Assets/Builtin/PlaylistPayload.hpp>

namespace bl
{
namespace asi
{
PlaylistPayload::PlaylistPayload(const as::Payload::ConstructContext& ctx)
: Payload(ctx)
, songs(ctx.repo, *this, "songs")
, shuffle(true)
, reshuffleOnLoop(true) {}

bool PlaylistPayload::addSong(util::UUID song) { return songs.addDependency(song); }

bool PlaylistPayload::removeSong(unsigned int i) { return songs.removeDependency(i); }

void PlaylistPayload::setShuffle(bool s) {
    shuffle = s;
    flush();
}

void PlaylistPayload::setReshuffleOnLoop(bool r) {
    reshuffleOnLoop = r;
    flush();
}

as::TypedRef<MusicPayload> PlaylistPayload::getSong(unsigned int i) const {
    return songs.getItem(i);
}

} // namespace asi
} // namespace bl
