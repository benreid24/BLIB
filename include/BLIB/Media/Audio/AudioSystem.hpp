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
class AudioSystem {
public:
    using Handle = std::uint32_t;

    static constexpr Handle InvalidHandle = 0;

    struct SpatialSettings {
        float fadeStartDistance;
        float attenuation;
    };

    struct Sound {
        const Handle handle;
        const Resource<sf::SoundBuffer>::Ref buffer;
        sf::Sound sound;

    private:
        Sound(Handle handle, Resource<sf::SoundBuffer>::Ref buffer);

        friend class AudioSystem;
    };

    AudioSystem();

    ~AudioSystem();

    void setDefaultSpatialSoundSettings(const SpatialSettings& settings);

    void setSpatialSoundCutoffDistance(float cutoff);

    void setVolume(float volume);

    void stopAll(bool fade = true);

    void pause();

    void resume();

    void setListenerPosition(const sf::Vector2f& pos);

    void pushPlaylist(const Playlist& newPlaylist, float fadeout = 4.f);

    void replacePlaylist(const Playlist& newPlaylist, float fadeout = 4.f);

    void popPlaylist(float fadeout = 4.f);

    Handle playSound(Resource<sf::SoundBuffer>::Ref sound, bool loop = false);

    Handle playSpatialSound(Resource<sf::SoundBuffer>::Ref sound, const sf::Vector2f& pos,
                            bool loop = false);

    Handle playSpatialSound(Resource<sf::SoundBuffer>::Ref sound, const sf::Vector2f& pos,
                            const SpatialSettings& settings, bool loop = false);

    std::shared_ptr<Sound> getSound(Handle handle);

    void stopSound(Handle handle);

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
    DynamicObjectPool<std::shared_ptr<Sound>> sounds;
    std::unordered_map<Handle, std::shared_ptr<Sound>> soundMap;
    SpatialSettings defaultSpatialSettings;
    float maxSpatialDistanceSquared;

    void background();
    float volume() const;
    Handle create() const;
};

} // namespace bl

#endif
