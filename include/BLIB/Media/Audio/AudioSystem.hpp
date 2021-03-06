#ifndef BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP
#define BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP

#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Media/Audio/Playlist.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Audio.hpp>

#include <cstdint>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bl
{
class AudioSystem {
public:
    using Handle = std::uint32_t;

    AudioSystem();

    ~AudioSystem();

    void setVolume(float volume);

    void stopAll(bool fade = true);

    void setListenerPosition(const sf::Vector2f& pos);

    void pushPlaylist(Playlist&& newPlaylist);

    void replacePlaylist(Playlist&& newPlaylist);

    void popPlaylist();

    Handle playSound(Resource<sf::Sound>::Ref sound);

    Handle playSpatialSound(Resource<sf::Sound>::Ref sound, const sf::Vector2f& pos);

    void stopSound(Handle handle);

private:
    float masterVolume;
    std::thread runner;

    std::mutex playlistMutex;
    std::vector<Playlist> playlists;
    enum MusicState { Playing, Pushing, Replacing, Popping } musicState;
    float musicVolumeFactor;

    std::mutex soundMutex;
    struct Sound {
        Handle handle;
        Resource<sf::SoundBuffer>::Ref buffer;
        sf::Sound sound;
    };
    DynamicObjectPool<Sound> sounds;
    std::unordered_map<Handle, DynamicObjectPool<Sound>::Iterator> soundIters;

    void background();
};

} // namespace bl

#endif
