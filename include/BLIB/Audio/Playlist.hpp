#ifndef BLIB_MEDIA_AUDIO_PLAYLIST_HPP
#define BLIB_MEDIA_AUDIO_PLAYLIST_HPP

#include <BLIB/Assets/Builtin/PlaylistPayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <BLIB/Streams.hpp>
#include <SFML/Audio/Music.hpp>
#include <string>
#include <vector>

namespace bl
{
namespace audio
{
/**
 * @brief Play state provider for playlist assets
 *
 * @ingroup Audio
 */
class Playlist {
public:
    /**
     * @brief Creates the playlist from the underlying asset
     *
     * @param asset The playlist asset
     */
    Playlist(as::TypedRef<asi::PlaylistPayload> asset);

    /**
     * @brief Returns whether or not the playlist is playing
     *
     * @return True if playing, false if paused or stopped
     */
    bool isPlaying() const;

    /**
     * @brief Plays or resumes the playlist. If set to shuffle and not resuming from pause, shuffles
     *        the songs before starting
     */
    void play();

    /**
     * @brief Pauses the playlist at the current time and current song. play() will resume from the
     *        pause point
     */
    void pause();

    /**
     * @brief Stops the playlist. play() will restart from the beginning
     */
    void stop();

    /**
     * @brief Updates the state of the playlist. This is called in the background if using
     *        AudioSystem. If not, then it must be called periodically. update() switches songs when
     *        current ones finish
     */
    void update();

    /**
     * @brief Set the volume
     *
     * @param volume Volume, [0,100]
     */
    void setVolume(float volume);

    /**
     * @brief Returns the current volume
     */
    float getVolume() const;

    /**
     * @brief Set whether or not the playlist should shuffle
     *
     * @param shuffle True to shuffle on play(), false to play in order
     */
    void setShuffle(bool shuffle);

    /**
     * @brief Returns whether or not the playlist shuffles on play()
     *
     * @return True if shuffling, false otherwise
     */
    bool shuffling() const;

    /**
     * @brief Set whether or not the Playlist reshuffles when it loops
     *
     * @param shuffleOnLoop True to shuffle again when complete, false to keep order
     */
    void setShuffleOnLoop(bool shuffleOnLoop);

    /**
     * @brief Returns whether or not the Playlist shuffles when it loops
     *
     * @return True if shuffling on each loop, false if keeping order
     */
    bool shufflingOnLoop() const;

private:
    as::TypedRef<asi::PlaylistPayload> asset;
    std::vector<unsigned int> playOrder;
    bool _shuffle;
    bool shuffleOnLoop;

    as::TypedRef<asi::MusicPayload> current;
    unsigned int currentIndex;
    unsigned int startIndex;
    bool playing, paused;

    void shuffle();
    bool openMusic(unsigned int i);
};

} // namespace audio
} // namespace bl

#endif
