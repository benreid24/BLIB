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

struct PlaylistFader {
    Playlist* playlist;
    float fvel;

    void update();
};

void initiateCrossfade(Playlist* in, Playlist* out, float inTime, float outTime);

class Runner {
public:
    Runner();
    ~Runner();
    void start();
    void stop();

    sf::Clock timer;
    float unloadTimeout = 120.f;

    std::unordered_map<std::string, AudioSystem::Handle> soundHandles;
    std::unordered_map<AudioSystem::Handle, Sound> sounds;
    std::list<SoundFader> fadingSounds;
    sf::Clock soundTimer;
    std::shared_mutex soundMutex;

    std::unordered_map<std::string, AudioSystem::Handle> playlistHandles;
    std::unordered_map<AudioSystem::Handle, Playlist> playlists;
    std::vector<Playlist*> playlistStack;
    sf::Clock fadeTimer;
    PlaylistFader fadeIn;
    PlaylistFader fadeOut;
    std::mutex playlistMutex;

    std::mutex pauseMutex;
    std::condition_variable pauseCond;
    bool paused;

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

void AudioSystem::setUnloadTimeout(float t) { runner.unloadTimeout = t; }

AudioSystem::Handle AudioSystem::getOrLoadSound(const std::string& path) {
    std::unique_lock lock(runner.soundMutex);

    auto it = runner.soundHandles.find(path);
    if (it != runner.soundHandles.end()) { return it->second; }

    const Handle handle = makeHandle();
    auto sit            = runner.sounds.try_emplace(handle).first;
    if (!sit->second.buffer.loadFromFile(path)) {
        runner.sounds.erase(sit);
        return InvalidHandle;
    }
    runner.soundHandles.emplace(path, handle);
    return handle;
}

bool AudioSystem::playSound(Handle sound, float fadeIn, bool loop) {
    std::shared_lock slock(runner.soundMutex);
    if (runner.paused) return false;

    const auto it = runner.sounds.find(sound);
    if (it == runner.sounds.end()) return false;

    it->second.sound.setLoop(loop);
    it->second.lastInteractTime = runner.timer.getElapsedTime().asSeconds();
    if (it->second.sound.getStatus() == sf::Sound::Playing) return true;

    if (fadeIn > 0.f) {
        it->second.sound.setVolume(0.f);
        runner.fadingSounds.emplace_back(it->second.sound, 100.f / fadeIn);
        runner.fadingSounds.back().me = --runner.fadingSounds.end();
    }
    else {
        it->second.sound.setVolume(100.f);
    }

    it->second.sound.play();
    return true;
}

void AudioSystem::stopSound(Handle sound, float fadeOut) {
    std::shared_lock slock(runner.soundMutex);

    const auto it = runner.sounds.find(sound);
    if (it != runner.sounds.end()) {
        it->second.lastInteractTime = runner.timer.getElapsedTime().asSeconds();
        if (it->second.sound.getStatus() == sf::Sound::Playing) {
            if (fadeOut <= 0.f) { it->second.sound.stop(); }
            else {
                runner.fadingSounds.emplace_back(it->second.sound,
                                                 -it->second.sound.getVolume() / fadeOut);
                runner.fadingSounds.back().me = --runner.fadingSounds.end();
            }
        }
    }
}

void AudioSystem::stopAllSounds() {
    std::unique_lock lock(runner.soundMutex);

    for (auto& sound : runner.sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Playing) { sound.second.sound.stop(); }
    }
}

AudioSystem::Handle AudioSystem::getOrLoadPlaylist(const std::string& path) {
    std::unique_lock lock(runner.playlistMutex);
    runner.start();

    const auto it = runner.playlistHandles.find(path);
    if (it != runner.playlistHandles.end()) return it->second;

    const Handle handle = makeHandle();
    auto pit            = runner.playlists.try_emplace(handle).first;
    if (!pit->second.load(path)) {
        runner.playlists.erase(pit);
        return InvalidHandle;
    }
    runner.playlistHandles.emplace(path, handle);
    return handle;
}

bool AudioSystem::pushPlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(runner.playlistMutex);
    runner.start();

    const auto it = runner.playlists.find(playlist);
    if (it == runner.playlists.end()) return false;

    initiateCrossfade(&it->second,
                      runner.playlistStack.empty() ? nullptr : runner.playlistStack.back(),
                      inTime,
                      outTime);
    runner.playlistStack.emplace_back(&it->second);
    return true;
}

bool AudioSystem::replacePlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(runner.playlistMutex);
    runner.start();

    const auto it = runner.playlists.find(playlist);
    if (it == runner.playlists.end()) return false;

    Playlist* out = nullptr;
    if (!runner.playlistStack.empty()) {
        out = runner.playlistStack.back();
        runner.playlistStack.pop_back();
    }
    initiateCrossfade(&it->second, out, inTime, outTime);
    runner.playlistStack.emplace_back(&it->second);
    return true;
}

void AudioSystem::popPlaylist(float inTime, float outTime) {
    std::unique_lock lock(runner.playlistMutex);
    if (runner.playlistStack.empty()) return;

    Playlist* in  = runner.playlistStack.size() >= 2 ?
                        runner.playlistStack[runner.playlistStack.size() - 2] :
                        nullptr;
    Playlist* out = runner.playlistStack.back();
    runner.playlistStack.pop_back();
    initiateCrossfade(in, out, inTime, outTime);
}

bool AudioSystem::replaceAllPlaylists(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(runner.playlistMutex);
    runner.start();

    const auto it = runner.playlists.find(playlist);
    if (it == runner.playlists.end()) return false;

    initiateCrossfade(&it->second,
                      runner.playlistStack.empty() ? nullptr : runner.playlistStack.back(),
                      inTime,
                      outTime);
    runner.playlistStack.clear();
    runner.playlistStack.emplace_back(&it->second);
    return true;
}

void AudioSystem::stopAllPlaylists(float outTime) {
    std::unique_lock lock(runner.playlistMutex);

    if (!runner.playlistStack.empty()) {
        initiateCrossfade(nullptr, runner.playlistStack.back(), 0.f, outTime);
        runner.playlistStack.clear();
    }
}

void AudioSystem::setVolume(float vol) { sf::Listener::setGlobalVolume(vol); }

void AudioSystem::pause() {
    std::unique_lock lock(runner.pauseMutex);
    if (runner.paused) return;
    runner.paused = true;

    for (auto& sound : runner.sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Playing) { sound.second.sound.pause(); }
    }
    if (!runner.playlistStack.empty() && runner.playlistStack.front()->isPlaying()) {
        runner.playlistStack.front()->pause();
    }
    if (runner.fadeIn.playlist && runner.fadeIn.playlist->isPlaying()) {
        runner.fadeIn.playlist->pause();
    }
    if (runner.fadeOut.playlist && runner.fadeOut.playlist->isPlaying()) {
        runner.fadeOut.playlist->pause();
    }
}

void AudioSystem::resume() {
    std::unique_lock lock(runner.pauseMutex);
    if (!runner.paused) return;
    runner.paused = false;

    for (auto& sound : runner.sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Paused) { sound.second.sound.play(); }
    }
    if (!runner.playlistStack.empty()) { runner.playlistStack.front()->play(); }
    if (runner.fadeIn.playlist) { runner.fadeIn.playlist->play(); }
    if (runner.fadeOut.playlist) { runner.fadeOut.playlist->play(); }

    runner.pauseCond.notify_all();
}

void AudioSystem::stop() {
    {
        std::unique_lock lock(runner.soundMutex);
        for (auto& sound : runner.sounds) { sound.second.sound.stop(); }
    }

    {
        std::unique_lock lock(runner.playlistMutex);
        for (auto p : runner.playlistStack) { p->stop(); }
        runner.playlistStack.clear();
        runner.fadeIn.playlist  = nullptr;
        runner.fadeOut.playlist = nullptr;
    }
}

void AudioSystem::shutdown() { runner.stop(); }

namespace
{
Sound::Sound()
: lastInteractTime(runner.timer.getElapsedTime().asSeconds()) {
    sound.setBuffer(buffer);
}

Runner::Runner()
: paused(false)
, running(false) {}

Runner::~Runner() { stop(); }

void Runner::start() {
    if (!running) {
        running = true;
        if (!thread.has_value()) { thread.emplace(&Runner::run, this); }
        BL_LOG_INFO << "Started AudioSystem";
    }
}

void Runner::stop() {
    BL_LOG_INFO << "Shutting down AudioSystem";
    running = false;
    if (thread.has_value() && thread.value().joinable()) { thread.value().join(); }
    AudioSystem::stop();
    BL_LOG_INFO << "AudioSystem shutdown";
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

        sf::sleep(sf::milliseconds(30));
    }

    BL_LOG_INFO << "AudioSystem thread terminated";
}

void initiateCrossfade(Playlist* in, Playlist* out, float inTime, float outTime) {
    if (runner.fadeIn.playlist) {
        runner.fadeIn.playlist->pause();
        runner.fadeIn.playlist = nullptr;
    }
    if (in) {
        if (inTime > 0.f) {
            runner.fadeIn.playlist = in;
            runner.fadeIn.fvel     = 100.f / inTime;
            in->setVolume(0.f);
        }
        else {
            in->setVolume(100.f);
        }
        if (!runner.paused) in->play();
    }

    if (runner.fadeOut.playlist) {
        runner.fadeOut.playlist->pause();
        runner.fadeOut.playlist = nullptr;
    }
    if (out) {
        if (outTime > 0.f) {
            runner.fadeOut.playlist = out;
            runner.fadeOut.fvel     = -out->getVolume() / outTime;
        }
        else {
            out->pause();
        }
    }
}

void PlaylistFader::update() {
    if (playlist) {
        const float vol =
            playlist->getVolume() + fvel * runner.fadeTimer.getElapsedTime().asSeconds();
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
    const float v = sound.getVolume() + fvel * runner.soundTimer.getElapsedTime().asSeconds();
    if (v <= 0.f) {
        sound.stop();
        runner.fadingSounds.erase(me);
    }
    else if (v >= 100.f) {
        sound.setVolume(100.f);
        runner.fadingSounds.erase(me);
    }
    else {
        sound.setVolume(v);
    }
}

} // namespace
} // namespace audio
} // namespace bl
