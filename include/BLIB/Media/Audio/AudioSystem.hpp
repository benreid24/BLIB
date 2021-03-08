#ifndef BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP
#define BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP

#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Media/Audio/Playlist.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Audio.hpp>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bl
{
/**
 * @brief Centralized control class for sound effects and music. It manages playing sounds, removing
 *        finished sounds, looping sounds, global volume, pausing, fading out on exit, and a stack
 *        of playlists.
 *
 * @ingroup Audio
 *
 */
class AudioSystem {
public:
    /// Integer type handle for playing sounds
    using Handle = std::uint32_t;

    /// Reserved Handle for error conditions
    static constexpr Handle InvalidHandle = 0;

    /**
     * @brief Some settings for spatial sounds
     *
     * @ingroup Audio
     *
     */
    struct SpatialSettings {
        /// The distance at which sounds should start getting quieter at
        float fadeStartDistance;

        /// The rate at which volume should fade with distance
        float attenuation;
    };

    /**
     * @brief Represents a sound being managed by AudioSystem
     *
     * @ingroup Audio
     *
     */
    struct Sound {
        /// The Handle of the playing sound
        const Handle handle;

        /// The underlying SoundBuffer being managed by a ResourceManager
        const Resource<sf::SoundBuffer>::Ref buffer;

        /// The actual sound that is playing
        sf::Sound sound;

    private:
        Sound(Handle handle, Resource<sf::SoundBuffer>::Ref buffer);

        friend class AudioSystem;
    };

    /**
     * @brief Stops all playing sounds and music with a one second fadeout, then terminates the
     *        background thread
     *
     */
    ~AudioSystem();

    /**
     * @brief Set the default settings to apply to spatial sounds
     *
     * @param settings The settings to apply to sounds without their own settings
     */
    static void setDefaultSpatialSoundSettings(const SpatialSettings& settings);

    /**
     * @brief Set the maximum distance at which spatial sounds will be played. Sounds coming from
     *        outside this distance will not play and return InvalidHandle
     *
     * @param cutoff The maximum distance. Default value is 320
     */
    static void setSpatialSoundCutoffDistance(float cutoff);

    /**
     * @brief Set the master volume of all audio. Spatial sounds will be attenuated as well
     *
     * @param volume The master volume for all audio
     */
    static void setVolume(float volume);

    /**
     * @brief Stops all playing sounds and playlists and removes them from the system
     *
     * @param fade True to fade the sound over one second, false to stop immediately
     */
    static void stopAll(bool fade = true);

    /**
     * @brief Pauses all sounds, music, and active fades/transitions
     *
     */
    static void pause();

    /**
     * @brief Resumes all sounds, music, and active fades/transitions
     *
     */
    static void resume();

    /**
     * @brief Sets the global position of the sound listener. This is used for attenuating spatial
     *        sounds. Should be the position of the player
     *
     * @param pos The position of the sound listener
     */
    static void setListenerPosition(const sf::Vector2f& pos);

    /**
     * @brief Pushes a new playlist onto the Playlist stack. If a playlist is currently playing it
     *        is crossfaded with the new playlist over the specified interval
     *
     * @param newPlaylist The new playlist to play
     * @param fadeout The interval to crossfade or fade in, in seconds
     */
    static void pushPlaylist(const Playlist& newPlaylist, float fadeout = 4.f);

    /**
     * @brief Replaces the playlist at the top of the stack. If a playlist is currently playing it
     *        is crossfaded with the new playlist over the specified interval
     *
     * @param newPlaylist The new playlist to play
     * @param fadeout The interval to crossfade or fade in, in seconds
     */
    static void replacePlaylist(const Playlist& newPlaylist, float fadeout = 4.f);

    /**
     * @brief Removes the currently playing playlist from the top of the stack and resumes the
     *        previous playlist if present
     *
     * @param fadeout The interval to crossfade or fade out, in seconds
     */
    static void popPlaylist(float fadeout = 4.f);

    /**
     * @brief Play a sound
     *
     * @param sound The sound to play
     * @param loop True to loop the sound, false to play once
     * @return Handle A Handle to the newly playing sound
     */
    static Handle playSound(Resource<sf::SoundBuffer>::Ref sound, bool loop = false);

    /**
     * @brief Play a sound in space with the default SpacialSettings
     *
     * @param sound The sound to play
     * @param pos The position of the sound
     * @param loop True to loop, false to play once
     * @return Handle A Handle to the newly playing sound, or InvalidHandle if too far away
     */
    static Handle playSpatialSound(Resource<sf::SoundBuffer>::Ref sound, const sf::Vector2f& pos,
                                   bool loop = false);

    /**
     * @brief Play a sound in space with its own SpatialSettings
     *
     * @param sound The sound to play
     * @param pos The position of the sound
     * @param settings The spaital settings of the sound
     * @param loop True to loop, false to play once
     * @return Handle A Handle to the newly playing sound, or InvalidHandle if too far away
     */
    static Handle playSpatialSound(Resource<sf::SoundBuffer>::Ref sound, const sf::Vector2f& pos,
                                   const SpatialSettings& settings, bool loop = false);

    /**
     * @brief Returns a shared_ptr to a currently running sound by Handle
     *
     * @param handle The Handle of the sound to get
     * @return std::shared_ptr<Sound> A pointer to the sound. May be null if handle is not found
     */
    static std::shared_ptr<Sound> getSound(Handle handle);

    /**
     * @brief Stops the currently playing sound. No effect if not found
     *
     * @param handle The handle of the sound to stop
     */
    static void stopSound(Handle handle);

private:
    struct SystemState {
        enum State { Running, Stopping, Paused };
    };

    std::atomic<float> masterVolume;
    std::thread runner;
    std::atomic<SystemState::State> state;
    std::atomic<float> fadeVolumeFactor;
    std::mutex pauseMutex;
    std::condition_variable pauseSync;

    std::mutex playlistMutex;
    std::vector<std::shared_ptr<Playlist>> playlists;
    struct MusicState {
        enum State { Playing, Pushing, Replacing, Popping, Stopped };
    };
    MusicState::State musicState;
    float musicVolumeFactor;
    float musicFadeAmount;

    std::shared_mutex soundMutex;
    container::DynamicObjectPool<std::shared_ptr<Sound>> sounds;
    std::unordered_map<Handle, std::shared_ptr<Sound>> soundMap;
    SpatialSettings defaultSpatialSettings;
    float maxSpatialDistanceSquared;

    AudioSystem();

    void setDefaultSpatialSoundSettingsImp(const SpatialSettings& settings);
    void setSpatialSoundCutoffDistanceImp(float cutoff);
    void setVolumeImp(float volume);
    void stopAllImp(bool fade);
    void pauseImp();
    void resumeImp();
    void setListenerPositionImp(const sf::Vector2f& pos);
    void pushPlaylistImp(const Playlist& newPlaylist, float fadeout);
    void replacePlaylistImp(const Playlist& newPlaylist, float fadeout);
    void popPlaylistImp(float fadeout = 4.f);
    Handle playSoundImp(Resource<sf::SoundBuffer>::Ref sound, bool loop);
    Handle playSpatialSoundImp(Resource<sf::SoundBuffer>::Ref sound, const sf::Vector2f& pos,
                               bool loop);
    Handle playSpatialSoundImp(Resource<sf::SoundBuffer>::Ref sound, const sf::Vector2f& pos,
                               const SpatialSettings& settings, bool loop);
    std::shared_ptr<Sound> getSoundImp(Handle handle);
    void stopSoundImp(Handle handle);

    void background();
    float volume() const;
    Handle create() const;

    static AudioSystem& get();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline void AudioSystem::setDefaultSpatialSoundSettings(const SpatialSettings& s) {
    get().setDefaultSpatialSoundSettingsImp(s);
}

inline void AudioSystem::setSpatialSoundCutoffDistance(float d) {
    get().setSpatialSoundCutoffDistanceImp(d);
}

inline void AudioSystem::setVolume(float v) { get().setVolumeImp(v); }

inline void AudioSystem::stopAll(bool f) { get().stopAllImp(f); }

inline void AudioSystem::pause() { get().pauseImp(); }

inline void AudioSystem::resume() { get().resumeImp(); }

inline void AudioSystem::setListenerPosition(const sf::Vector2f& pos) {
    get().setListenerPositionImp(pos);
}

inline void AudioSystem::pushPlaylist(const Playlist& p, float f) { get().pushPlaylistImp(p, f); }

inline void AudioSystem::replacePlaylist(const Playlist& p, float f) {
    get().replacePlaylistImp(p, f);
}

inline void AudioSystem::popPlaylist(float f) { get().popPlaylistImp(f); }

inline AudioSystem::Handle AudioSystem::playSound(Resource<sf::SoundBuffer>::Ref sound, bool loop) {
    return get().playSoundImp(sound, loop);
}

inline AudioSystem::Handle AudioSystem::playSpatialSound(Resource<sf::SoundBuffer>::Ref sound,
                                                         const sf::Vector2f& pos, bool loop) {
    return get().playSpatialSoundImp(sound, pos, loop);
}

inline AudioSystem::Handle AudioSystem::playSpatialSound(Resource<sf::SoundBuffer>::Ref sound,
                                                         const sf::Vector2f& pos,
                                                         const SpatialSettings& settings,
                                                         bool loop) {
    return get().playSpatialSoundImp(sound, pos, settings, loop);
}

inline std::shared_ptr<AudioSystem::Sound> AudioSystem::getSound(Handle h) {
    return get().getSoundImp(h);
}

inline void AudioSystem::stopSound(Handle h) { get().stopSoundImp(h); }

} // namespace bl

#endif
