#ifndef BLIB_ASSETS_BUILTIN_PLAYLISTPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_PLAYLISTPAYLOAD_HPP

#include <BLIB/Assets/Builtin/MusicPayload.hpp>
#include <BLIB/Assets/DependencyList.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload for a music playlist asset
 *
 * @ingroup Assets
 */
class PlaylistPayload : public as::Payload {
public:
    /**
     * @brief Creates an empty playlist payload
     * @param ctx The context to construct with
     */
    PlaylistPayload(const as::Payload::ConstructContext& ctx);

    /**
     * @brief Destroys the payload
     */
    virtual ~PlaylistPayload() = default;

    /**
     * @brief Adds a song to the playlist
     *
     * @param song The UUID of the song to add
     * @return True if the song was successfully added, false otherwise
     */
    bool addSong(util::UUID song);

    /**
     * @brief Removes the song at the given index from the playlist
     *
     * @param i The index of the song to remove
     * @return True if the song was successfully removed, false otherwise
     */
    bool removeSong(unsigned int i);

    /**
     * @brief Sets whether the playlist should shuffle on play
     *
     * @param shuffle True to shuffle, false to play in order
     */
    void setShuffle(bool shuffle);

    /**
     * @brief Sets whether the playlist should reshuffle when it loops
     *
     * @param reshuffleOnLoop True to reshuffle on loop, false to keep order
     */
    void setReshuffleOnLoop(bool reshuffleOnLoop);

    /**
     * @brief Returns the playlist shuffle setting
     */
    bool getShuffle() const { return shuffle; }

    /**
     * @brief Returns the playlist reshuffle on loop setting
     */
    bool getReshuffleOnLoop() const { return reshuffleOnLoop; }

    /**
     * @brief Returns the number of songs in the playlist
     */
    unsigned int getSongCount() const { return songs.getSize(); }

    /**
     * @brief Returns a ref to the song at the given index
     *
     * @param i The index of the song to fetch
     * @return A ref to the asset for the song
     */
    as::TypedRef<MusicPayload> getSong(unsigned int i) const;

private:
    as::DependencyList<MusicPayload> songs;
    bool shuffle;
    bool reshuffleOnLoop;

    friend struct serial::SerializableObject<PlaylistPayload>;
    friend struct refl::ReflectedObject<PlaylistPayload>;
};

} // namespace asi

namespace serial
{
template<>
struct SerializableObject<asi::PlaylistPayload> : public SerializableObjectBase {
    SerializableField<1, asi::PlaylistPayload, bool> shuffle;
    SerializableField<2, asi::PlaylistPayload, bool> reshuffleOnLoop;

    SerializableObject()
    : SerializableObjectBase("Playlist")
    , shuffle("shuffle", *this, &asi::PlaylistPayload::shuffle, SerializableFieldBase::Required{})
    , reshuffleOnLoop("reshuffleOnLoop", *this, &asi::PlaylistPayload::reshuffleOnLoop,
                      SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<asi::PlaylistPayload> {
    inline static const auto spec = makeSpec<asi::PlaylistPayload>(
        "Playlist",
        memberList(defineMember(1, "shuffle", &asi::PlaylistPayload::shuffle),
                   defineMember(2, "reshuffleOnLoop", &asi::PlaylistPayload::reshuffleOnLoop)));
};
} // namespace refl

} // namespace bl

#endif
