#include <BLIB/Media/Audio/AudioSystem.hpp>

#include <BLIB/Util/Random.hpp>
#include <SFML/Audio.hpp>
#include <atomic>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

namespace bl
{
namespace audio
{
namespace
{
sf::Clock timer;
float unloadTimeout = 120.f;

struct Sound {
    sf::SoundBuffer buffer;
    sf::Sound sound;
    float lastInteractTime;

    Sound();
};

struct SoundFader {
    sf::Sound& sound;
    const float fvel;
    std::list<SoundFader>::iterator me;

    SoundFader(sf::Sound& s, float fvel);
    void update();
};

std::unordered_map<std::string, AudioSystem::Handle> soundHandles;
std::unordered_map<AudioSystem::Handle, Sound> sounds;
std::list<SoundFader> fadingSounds;
sf::Clock soundTimer;
std::shared_mutex soundMutex;

struct PlaylistFader {
    Playlist* playlist;
    float fvel;

    void update();
};

std::unordered_map<std::string, AudioSystem::Handle> playlistHandles;
std::unordered_map<AudioSystem::Handle, Playlist> playlists;
std::vector<Playlist*> playlistStack;
sf::Clock fadeTimer;
PlaylistFader fadeIn;
PlaylistFader fadeOut;
std::mutex playlistMutex;

std::mutex pauseMutex;
std::condition_variable pauseCond;
bool paused = false;

void initiateCrossfade(Playlist* in, Playlist* out, float inTime, float outTime);

class Runner {
public:
    Runner();
    ~Runner();
    void start();

private:
    std::optional<std::thread> thread;
    std::atomic<bool> running;

    void run();
} runner;

inline AudioSystem::Handle makeHandle() {
    return util::Random::get<AudioSystem::Handle>(1,
                                                  std::numeric_limits<AudioSystem::Handle>::max());
}
} // namespace

void AudioSystem::setUnloadTimeout(float t) { unloadTimeout = t; }

AudioSystem::Handle AudioSystem::getOrLoadSound(const std::string& path) {
    std::unique_lock lock(soundMutex);

    auto it = soundHandles.find(path);
    if (it != soundHandles.end()) { return it->second; }

    const Handle handle = makeHandle();
    auto sit            = sounds.try_emplace(handle).first;
    if (!sit->second.buffer.loadFromFile(path)) {
        sounds.erase(sit);
        return InvalidHandle;
    }
    soundHandles.emplace(path, handle);
    return handle;
}

bool AudioSystem::playSound(Handle sound, float fadeIn, bool loop) {
    std::shared_lock slock(soundMutex);
    if (paused) return false;

    const auto it = sounds.find(sound);
    if (it == sounds.end()) return false;

    it->second.sound.setLoop(loop);
    it->second.lastInteractTime = timer.getElapsedTime().asSeconds();
    if (it->second.sound.getStatus() == sf::Sound::Playing) return true;

    if (fadeIn > 0.f) {
        it->second.sound.setVolume(0.f);
        fadingSounds.emplace_back(it->second.sound, 100.f / fadeIn);
    }
    else {
        it->second.sound.setVolume(100.f);
    }

    it->second.sound.play();
    return true;
}

void AudioSystem::stopSound(Handle sound, float fadeOut) {
    std::shared_lock slock(soundMutex);

    const auto it = sounds.find(sound);
    if (it != sounds.end()) {
        it->second.lastInteractTime = timer.getElapsedTime().asSeconds();
        if (it->second.sound.getStatus() == sf::Sound::Playing) {
            if (fadeOut <= 0.f) { it->second.sound.stop(); }
            else {
                fadingSounds.emplace_back(it->second.sound,
                                          -it->second.sound.getVolume() / fadeOut);
                fadingSounds.back().me = --fadingSounds.end();
            }
        }
    }
}

void AudioSystem::stopAllSounds() {
    std::unique_lock lock(soundMutex);

    for (auto& sound : sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Playing) { sound.second.sound.stop(); }
    }
}

AudioSystem::Handle AudioSystem::getOrLoadPlaylist(const std::string& path) {
    std::unique_lock lock(playlistMutex);
    runner.start();

    const auto it = playlistHandles.find(path);
    if (it != playlistHandles.end()) return it->second;

    const Handle handle = makeHandle();
    auto pit            = playlists.try_emplace(handle).first;
    if (!pit->second.load(path)) {
        playlists.erase(pit);
        return InvalidHandle;
    }
    playlistHandles.emplace(path, handle);
    return handle;
}

bool AudioSystem::pushPlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(playlistMutex);
    runner.start();

    const auto it = playlists.find(playlist);
    if (it == playlists.end()) return false;

    initiateCrossfade(
        &it->second, playlistStack.empty() ? nullptr : playlistStack.back(), inTime, outTime);
    playlistStack.emplace_back(&it->second);
    return true;
}

bool AudioSystem::replacePlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(playlistMutex);
    runner.start();

    const auto it = playlists.find(playlist);
    if (it == playlists.end()) return false;

    Playlist* out = nullptr;
    if (!playlistStack.empty()) {
        out = playlistStack.back();
        playlistStack.pop_back();
    }
    initiateCrossfade(&it->second, out, inTime, outTime);
    playlistStack.emplace_back(&it->second);
    return true;
}

void AudioSystem::popPlaylist(float inTime, float outTime) {
    std::unique_lock lock(playlistMutex);
    if (playlistStack.empty()) return;

    Playlist* in  = playlistStack.size() >= 2 ? playlistStack[playlistStack.size() - 2] : nullptr;
    Playlist* out = playlistStack.back();
    playlistStack.pop_back();
    initiateCrossfade(in, out, inTime, outTime);
}

bool AudioSystem::replaceAllPlaylists(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(playlistMutex);
    runner.start();

    const auto it = playlists.find(playlist);
    if (it == playlists.end()) return false;

    initiateCrossfade(
        &it->second, playlistStack.empty() ? nullptr : playlistStack.back(), inTime, outTime);
    playlistStack.clear();
    playlistStack.emplace_back(&it->second);
    return true;
}

void AudioSystem::stopAllPlaylists(float outTime) {
    std::unique_lock lock(playlistMutex);

    if (!playlistStack.empty()) {
        initiateCrossfade(nullptr, playlistStack.back(), 0.f, outTime);
        playlistStack.clear();
    }
}

void AudioSystem::setVolume(float vol) { sf::Listener::setGlobalVolume(vol); }

void AudioSystem::pause() {
    std::unique_lock lock(pauseMutex);
    if (paused) return;
    paused = true;

    for (auto& sound : sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Playing) { sound.second.sound.pause(); }
    }
    if (!playlistStack.empty() && playlistStack.front()->isPlaying()) {
        playlistStack.front()->pause();
    }
    if (fadeIn.playlist && fadeIn.playlist->isPlaying()) { fadeIn.playlist->pause(); }
    if (fadeOut.playlist && fadeOut.playlist->isPlaying()) { fadeOut.playlist->pause(); }
}

void AudioSystem::resume() {
    std::unique_lock lock(pauseMutex);
    if (!paused) return;
    paused = false;

    for (auto& sound : sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Paused) { sound.second.sound.play(); }
    }
    if (!playlistStack.empty()) { playlistStack.front()->play(); }
    if (fadeIn.playlist) { fadeIn.playlist->play(); }
    if (fadeOut.playlist) { fadeOut.playlist->play(); }

    pauseCond.notify_all();
}

void AudioSystem::stop() {
    {
        std::unique_lock lock(soundMutex);
        for (auto& sound : sounds) { sound.second.sound.stop(); }
    }

    {
        std::unique_lock lock(playlistMutex);
        for (auto p : playlistStack) { p->stop(); }
        playlistStack.clear();
        fadeIn.playlist  = nullptr;
        fadeOut.playlist = nullptr;
    }
}

namespace
{
Sound::Sound()
: lastInteractTime(timer.getElapsedTime().asSeconds()) {
    sound.setBuffer(buffer);
}

Runner::Runner()
: running(false) {}

Runner::~Runner() {
    running = false;
    if (thread.has_value()) { thread.value().join(); }
}

void Runner::start() {
    if (!running) {
        running = true;
        if (!thread.has_value()) { thread.emplace(&Runner::run, this); }
    }
}

void Runner::run() {
    while (running) {
        {
            std::unique_lock plock(pauseMutex);
            if (paused) { pauseCond.wait(plock); }

            // sound cleanup and fadeouts
            {
                std::unique_lock slock(soundMutex);

                for (auto i = soundHandles.begin(); i != soundHandles.end();) {
                    auto j  = i++;
                    auto it = sounds.find(j->second);
                    if (it != sounds.end()) {
                        if (timer.getElapsedTime().asSeconds() - it->second.lastInteractTime >=
                            unloadTimeout + it->second.buffer.getDuration().asSeconds()) {
                            sounds.erase(it);
                        }
                    }
                    soundHandles.erase(j);
                }

                for (auto i = fadingSounds.begin(); i != fadingSounds.end();) {
                    auto j = i++;
                    j->update();
                }
                soundTimer.restart();
            }

            // playlist crossfade update and song update
            {
                std::unique_lock plock(playlistMutex);

                if (!playlistStack.empty() && playlistStack.front()->isPlaying()) {
                    playlistStack.front()->update();
                }

                fadeIn.update();
                fadeOut.update();
                fadeTimer.restart();
            }
        }

        sf::sleep(sf::milliseconds(300));
    }
}

void initiateCrossfade(Playlist* in, Playlist* out, float inTime, float outTime) {
    if (fadeIn.playlist) {
        fadeIn.playlist->pause();
        fadeIn.playlist = nullptr;
    }
    if (in) {
        if (inTime > 0.f) {
            fadeIn.playlist = in;
            fadeIn.fvel     = 100.f / inTime;
            in->setVolume(0.f);
        }
        else {
            in->setVolume(100.f);
        }
        if (!paused) in->play();
    }

    if (fadeOut.playlist) {
        fadeOut.playlist->pause();
        fadeOut.playlist = nullptr;
    }
    if (out) {
        if (outTime > 0.f) {
            fadeOut.playlist = out;
            fadeOut.fvel     = -out->getVolume() / outTime;
        }
        else {
            out->pause();
        }
    }
}

void PlaylistFader::update() {
    if (playlist) {
        const float vol = playlist->getVolume() + fvel * fadeTimer.getElapsedTime().asSeconds();
        if (vol <= 0.f) {
            playlist->setVolume(0.f);
            playlist->pause();
            playlist = nullptr;
        }
        else if (vol >= 100.f) {
            playlist->setVolume(100.f);
            playlist = nullptr;
        }
        else {
            playlist->setVolume(vol);
        }
    }
}

SoundFader::SoundFader(sf::Sound& s, float v)
: sound(s)
, fvel(v) {}

void SoundFader::update() {
    const float v = sound.getVolume() + fvel * soundTimer.getElapsedTime().asSeconds();
    if (v <= 0.f) {
        sound.stop();
        fadingSounds.erase(me);
    }
    else if (v >= 100.f) {
        sound.setVolume(100.f);
        fadingSounds.erase(me);
    }
    else {
        sound.setVolume(v);
    }
}

} // namespace
} // namespace audio
} // namespace bl
