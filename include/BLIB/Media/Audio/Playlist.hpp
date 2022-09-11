#ifndef BLIB_MEDIA_AUDIO_PLAYLIST_HPP
#define BLIB_MEDIA_AUDIO_PLAYLIST_HPP

#include <BLIB/Serialization.hpp>
#include <SFML/Audio/Music.hpp>

#include <string>
#include <vector>

namespace bl
{
namespace audio
{
/**
 * @brief A playable, serializable music playlist. Can be manually manipulated or loaded from files
 *
 * @ingroup Audio
 *
 */
class Playlist {
public:
    /**
     * @brief Creates an empty playlist
     *
     */
    Playlist();

    /**
     * @brief Loads the playlist from the given file
     *
     * @param source The file to load from
     */
    Playlist(const std::string& source);

    /**
     * @brief Copies the song list and shuffle setting from the given playlist, but not it's state
     *
     * @param copy The playlist to duplicate
     *
     */
    Playlist(const Playlist& copy);

    /**
     * @brief Copies the song list and shuffle setting from the given playlist, but not it's state
     *
     * @param copy The playlist to duplicate
     *
     */
    Playlist& operator=(const Playlist& copy);

    /**
     * @brief Loads the playlist from the given binary file
     *
     * @param path The file to load from
     * @return True on success, false on error
     */
    bool load(const std::string& path);

    /**
     * @brief Saves the conversation to the given binary file
     *
     * @param path The file to save to
     * @return True if the conversation could be saved, false on error
     */
    bool save(const std::string& path) const;

    /**
     * @brief Returns whether or not the playlist is playing
     *
     * @return True if playing, false if paused or stopped
     */
    bool isPlaying() const;

    /**
     * @brief Plays or resumes the playlist. If set to shuffle and not resuming from pause, shuffles
     *        the songs before starting
     *
     */
    void play();

    /**
     * @brief Pauses the playlist at the current time and current song. play() will resume from the
     *        pause point
     *
     */
    void pause();

    /**
     * @brief Stops the playlist. play() will restart from the beginning
     *
     */
    void stop();

    /**
     * @brief Updates the state of the playlist. This is called in the background if using
     *        AudioSystem. If not, then it must be called periodically. update() switches songs when
     *        current ones finish
     *
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
     *
     */
    float getVolume() const;

    /**
     * @brief Add a song to the playlist to be played. Not threadsafe
     *
     * @param song The filename of the song to play
     */
    void addSong(const std::string& song);

    /**
     * @brief Removes the song from the queue
     *
     * @param song The filename of the song to remove
     */
    void removeSong(const std::string& song);

    /**
     * @brief Returns the immutable list of songs in the queue in their shuffled order
     *
     * @return const std::vector<std::string>& The list of song filenames
     */
    const std::vector<std::string>& getSongList() const;

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
    std::vector<std::string> songs;
    bool _shuffle;
    bool shuffleOnLoop;

    sf::Music current;
    unsigned int currentIndex;
    unsigned int startIndex;
    bool playing, paused;

    void shuffle();

    friend class serial::SerializableObject<audio::Playlist>;
};

} // namespace audio

namespace serial
{
template<>
struct SerializableObject<audio::Playlist> : public SerializableObjectBase {
    SerializableField<1, audio::Playlist, std::vector<std::string>> songs;
    SerializableField<2, audio::Playlist, bool> shuffle;
    SerializableField<3, audio::Playlist, bool> shuffleOnLoop;

    SerializableObject()
    : songs("songs", *this, &audio::Playlist::songs, SerializableFieldBase::Required{})
    , shuffle("shuffle", *this, &audio::Playlist::_shuffle, SerializableFieldBase::Optional{})
    , shuffleOnLoop("shuffleOnLoop", *this, &audio::Playlist::shuffleOnLoop,
                    SerializableFieldBase::Optional{}) {
        shuffle.setDefault(true);
        shuffleOnLoop.setDefault(true);
    }
};

} // namespace serial
} // namespace bl

#endif
