#ifndef BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP
#define BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Media/Audio/Playlist.hpp>
#include <SFML/Audio.hpp>
#include <cstdint>

namespace bl
{
/// Audio utilities, tools, and functionality
namespace audio
{
/**
 * @brief Centralized audio system for playing sounds and music
 *
 * @ingroup Audio
 *
 */
class AudioSystem {
public:
    /// Identifier of a sound or playlist
    using Handle = std::uint32_t;

    /// Special value to indicate an error loading a sound or playlist
    static constexpr Handle InvalidHandle = 0;

    /**
     * @brief Sets the amount of time sounds should be in memory before being cleared
     *
     * @param seconds The time to keep sounds loaded. Default is two minutes
     */
    static void setUnloadTimeout(float seconds);

    /**
     * @brief Loads a sound and returns a handle for it. Sounds are loaded at most once
     *
     * @param path The file to load from
     * @return Handle The handle to the loaded or existing sound. May be InvalidHandle
     */
    static Handle getOrLoadSound(const std::string& path);

    /**
     * @brief Plays the sound with the given handle
     *
     * @param sound The sound to play
     * @param loop True to loop, false to play once
     * @return True if the sound was able to be played, false if not
     */
    static bool playSound(Handle sound, float fadeIn = -1.f, bool loop = false);

    /**
     * @brief Immediately plays or restarts the given sound, without fade-in and no looping
     *
     * @param sound The sound to play or restart
     * @return True if the sound could be played, false if not found
     */
    static bool playOrRestartSound(Handle sound);

    /**
     * @brief Stops the given sound if it is playing
     *
     * @param sound The sound to stop
     * @param fade Optional fade out time in seconds
     */
    static void stopSound(Handle sound, float fade = -1.f);

    /**
     * @brief Stops all sounds immediately
     *
     */
    static void stopAllSounds();

    /**
     * @brief Loads a playlist and returns a handle for it. Playlists are loaded at most once
     *
     * @param path The file to load from
     * @return Handle The handle to the loaded or existing playlist. May be InvalidHandle
     */
    static Handle getOrLoadPlaylist(const std::string& path);

    /**
     * @brief Begins playing the new playlist with the optional crossfade. Previously playing
     *        playlists are preverved
     *
     * @param playlist The playlist to play
     * @param fadeIn Fade in time of the new playlist
     * @param fadeOut Fade out time of the previously playing playlist, if any
     * @return True if the playlist was able to be played, false on error
     */
    static bool pushPlaylist(Handle playlist, float fadeIn = 2.f, float fadeOut = 2.f);

    /**
     * @brief Stops playing the current playlist and resumes the previous, if any
     *
     * @param fadeIn Fade in time of the previous playlist
     * @param fadeOut Fade out time of the current playlist
     */
    static void popPlaylist(float fadeIn = 2.f, float fadeOut = 2.f);

    /**
     * @brief Replaces the currently playing playlist with the new playlist
     *
     * @param playlist The new playlist to play
     * @param fadeIn Fade in time of the new playlist
     * @param fadeOut Fade out time of the current playlist
     * @return True if the playlist was able to be played, false otherwise
     */
    static bool replacePlaylist(Handle playlist, float fadeIn = 2.f, float fadeOut = 2.f);

    /**
     * @brief Replaces all previous and currently playing playlists with the new playlist
     *
     * @param playlist The new playlist to play
     * @param fadeIn Fade in time of the new playlist
     * @param fadeOut Fade out time of the previously playing playlist
     * @return True if the new playlist could be played, false otherwise
     */
    static bool replaceAllPlaylists(Handle playlist, float fadeIn = 2.f, float fadeOut = 2.f);

    /**
     * @brief Stops all current and previous playlists
     *
     * @param fadeOut Fade out time of currently playing playlist
     */
    static void stopAllPlaylists(float fadeOut = -1.f);

    /**
     * @brief Sets the global volume of all playlists and sounds
     *
     * @param volume The global volume, in the range [0, 100]
     */
    static void setVolume(float volume);

    /**
     * @brief Returns the global volume
     *
     * @return float The global volume, in range [0, 100]
     */
    static float getVolume();

    /**
     * @brief Sets whether or not audio is muted
     *
     * @param muted True to mute, false to play audio at the previously set volume
     */
    static void setMuted(bool muted);

    /**
     * @brief Returns whether or not audio is muted. Volume of 0 does not count as muted
     *
     * @return True if audio is muted, false if it is not
     */
    static bool getMuted();

    /**
     * @brief Loads system settings from the engine config
     *
     */
    static void loadFromConfig();

    /**
     * @brief Saves system settings to the engine config
     *
     */
    static void saveToConfig();

    /**
     * @brief Pauses all sounds and playlists
     *
     */
    static void pause();

    /**
     * @brief Resumes all paused sounds and playlists
     *
     */
    static void resume();

    /**
     * @brief Stops all sounds and playlists
     *
     */
    static void stop();

    /**
     * @brief This should be called at the end of main() if the audiosystem is used. Note that this
     *        is called by Engine if it is used
     *
     * @param fade True to fade out all sound, false to hard stop
     *
     */
    static void shutdown(bool fade = true);
};

} // namespace audio
} // namespace bl

#endif
