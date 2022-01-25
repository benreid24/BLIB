#ifndef BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP
#define BLIB_MEDIA_AUDIO_AUDIOSYSTEM_HPP

#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Media/Audio/Playlist.hpp>
#include <SFML/Audio.hpp>
#include <cstdint>

namespace bl
{
namespace audio
{
class AudioSystem {
public:
    using Handle = std::uint32_t;

    static constexpr Handle InvalidHandle = 0;

    static void setUnloadTimeout(float seconds);

    static Handle getOrLoadSound(const std::string& path);

    static bool playSound(Handle sound, bool loop = false);

    static void stopSound(Handle sound);

    static void stopAllSounds();

    static Handle getOrLoadPlaylist(const std::string& path);

    static bool pushPlaylist(Handle playlist, float fadeIn = 2.f, float fadeOut = 2.f);

    static void popPlaylist(float fadeIn = 2.f, float fadeOut = 2.f);

    static bool replacePlaylist(Handle playlist, float fadeIn = 2.f, float fadeOut = 2.f);

    static bool replaceAllPlaylists(Handle playlist, float fadeIn = 2.f, float fadeOut = 2.f);

    static void stopAllPlaylists(float fadeOut = -1.f);

    static void setVolume(float volume);

    static void pause();

    static void resume();

    static void stop();
};

} // namespace audio
} // namespace bl

#endif
