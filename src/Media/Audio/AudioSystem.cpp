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
    static Runner& get();
    static void stop();

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
    Runner();
    ~Runner();

    std::thread thread;
    static std::atomic<bool> started;
    std::atomic<bool> shouldStop;

    void run();
    void shutdown();
};

std::atomic<bool> Runner::started = false;

inline AudioSystem::Handle makeHandle() {
    return util::Random::get<AudioSystem::Handle>(1,
                                                  std::numeric_limits<AudioSystem::Handle>::max());
}
} // namespace

void AudioSystem::setUnloadTimeout(float t) { Runner::get().unloadTimeout = t; }

AudioSystem::Handle AudioSystem::getOrLoadSound(const std::string& path) {
    std::unique_lock lock(Runner::get().soundMutex);

    auto it = Runner::get().soundHandles.find(path);
    if (it != Runner::get().soundHandles.end()) { return it->second; }

    const Handle handle = makeHandle();
    auto sit            = Runner::get().sounds.try_emplace(handle).first;
    if (!sit->second.buffer.loadFromFile(path)) {
        Runner::get().sounds.erase(sit);
        return InvalidHandle;
    }
    Runner::get().soundHandles.emplace(path, handle);
    return handle;
}

bool AudioSystem::playSound(Handle sound, float fadeIn, bool loop) {
    std::shared_lock slock(Runner::get().soundMutex);
    if (Runner::get().paused) return false;

    const auto it = Runner::get().sounds.find(sound);
    if (it == Runner::get().sounds.end()) return false;

    it->second.sound.setLoop(loop);
    it->second.lastInteractTime = Runner::get().timer.getElapsedTime().asSeconds();
    if (it->second.sound.getStatus() == sf::Sound::Playing) return true;

    if (fadeIn > 0.f) {
        it->second.sound.setVolume(0.f);
        Runner::get().fadingSounds.emplace_back(it->second.sound, 100.f / fadeIn);
        Runner::get().fadingSounds.back().me = --Runner::get().fadingSounds.end();
    }
    else {
        it->second.sound.setVolume(100.f);
    }

    it->second.sound.play();
    BL_LOG_INFO << "played";
    return true;
}

void AudioSystem::stopSound(Handle sound, float fadeOut) {
    std::shared_lock slock(Runner::get().soundMutex);

    const auto it = Runner::get().sounds.find(sound);
    if (it != Runner::get().sounds.end()) {
        it->second.lastInteractTime = Runner::get().timer.getElapsedTime().asSeconds();
        if (it->second.sound.getStatus() == sf::Sound::Playing) {
            if (fadeOut <= 0.f) { it->second.sound.stop(); }
            else {
                Runner::get().fadingSounds.emplace_back(it->second.sound,
                                                        -it->second.sound.getVolume() / fadeOut);
                Runner::get().fadingSounds.back().me = --Runner::get().fadingSounds.end();
            }
        }
    }
}

void AudioSystem::stopAllSounds() {
    std::unique_lock lock(Runner::get().soundMutex);

    for (auto& sound : Runner::get().sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Playing) { sound.second.sound.stop(); }
    }
}

AudioSystem::Handle AudioSystem::getOrLoadPlaylist(const std::string& path) {
    std::unique_lock lock(Runner::get().playlistMutex);

    const auto it = Runner::get().playlistHandles.find(path);
    if (it != Runner::get().playlistHandles.end()) return it->second;

    const Handle handle = makeHandle();
    auto pit            = Runner::get().playlists.try_emplace(handle).first;
    if (!pit->second.load(path)) {
        Runner::get().playlists.erase(pit);
        return InvalidHandle;
    }
    Runner::get().playlistHandles.emplace(path, handle);
    return handle;
}

bool AudioSystem::pushPlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    const auto it = Runner::get().playlists.find(playlist);
    if (it == Runner::get().playlists.end()) return false;

    initiateCrossfade(&it->second,
                      Runner::get().playlistStack.empty() ? nullptr :
                                                            Runner::get().playlistStack.back(),
                      inTime,
                      outTime);
    Runner::get().playlistStack.emplace_back(&it->second);
    return true;
}

bool AudioSystem::replacePlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    const auto it = Runner::get().playlists.find(playlist);
    if (it == Runner::get().playlists.end()) return false;

    Playlist* out = nullptr;
    if (!Runner::get().playlistStack.empty()) {
        out = Runner::get().playlistStack.back();
        Runner::get().playlistStack.pop_back();
    }
    initiateCrossfade(&it->second, out, inTime, outTime);
    Runner::get().playlistStack.emplace_back(&it->second);
    return true;
}

void AudioSystem::popPlaylist(float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);
    if (Runner::get().playlistStack.empty()) return;

    Playlist* in  = Runner::get().playlistStack.size() >= 2 ?
                        Runner::get().playlistStack[Runner::get().playlistStack.size() - 2] :
                        nullptr;
    Playlist* out = Runner::get().playlistStack.back();
    Runner::get().playlistStack.pop_back();
    initiateCrossfade(in, out, inTime, outTime);
}

bool AudioSystem::replaceAllPlaylists(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    const auto it = Runner::get().playlists.find(playlist);
    if (it == Runner::get().playlists.end()) return false;

    initiateCrossfade(&it->second,
                      Runner::get().playlistStack.empty() ? nullptr :
                                                            Runner::get().playlistStack.back(),
                      inTime,
                      outTime);
    Runner::get().playlistStack.clear();
    Runner::get().playlistStack.emplace_back(&it->second);
    return true;
}

void AudioSystem::stopAllPlaylists(float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    if (!Runner::get().playlistStack.empty()) {
        initiateCrossfade(nullptr, Runner::get().playlistStack.back(), 0.f, outTime);
        Runner::get().playlistStack.clear();
    }
}

void AudioSystem::setVolume(float vol) { sf::Listener::setGlobalVolume(vol); }

void AudioSystem::pause() {
    std::unique_lock lock(Runner::get().pauseMutex);
    if (Runner::get().paused) return;
    Runner::get().paused = true;

    for (auto& sound : Runner::get().sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Playing) { sound.second.sound.pause(); }
    }
    if (!Runner::get().playlistStack.empty() && Runner::get().playlistStack.front()->isPlaying()) {
        Runner::get().playlistStack.front()->pause();
    }
    if (Runner::get().fadeIn.playlist && Runner::get().fadeIn.playlist->isPlaying()) {
        Runner::get().fadeIn.playlist->pause();
    }
    if (Runner::get().fadeOut.playlist && Runner::get().fadeOut.playlist->isPlaying()) {
        Runner::get().fadeOut.playlist->pause();
    }
}

void AudioSystem::resume() {
    std::unique_lock lock(Runner::get().pauseMutex);
    if (!Runner::get().paused) return;
    Runner::get().paused = false;

    for (auto& sound : Runner::get().sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Paused) { sound.second.sound.play(); }
    }
    if (!Runner::get().playlistStack.empty()) { Runner::get().playlistStack.front()->play(); }
    if (Runner::get().fadeIn.playlist) { Runner::get().fadeIn.playlist->play(); }
    if (Runner::get().fadeOut.playlist) { Runner::get().fadeOut.playlist->play(); }

    Runner::get().pauseCond.notify_all();
}

void AudioSystem::stop() {
    {
        std::unique_lock lock(Runner::get().soundMutex);
        for (auto& sound : Runner::get().sounds) { sound.second.sound.stop(); }
    }

    {
        std::unique_lock lock(Runner::get().playlistMutex);
        for (auto p : Runner::get().playlistStack) { p->stop(); }
        Runner::get().playlistStack.clear();
        Runner::get().fadeIn.playlist  = nullptr;
        Runner::get().fadeOut.playlist = nullptr;
    }
}

void AudioSystem::shutdown() { Runner::stop(); }

namespace
{
Sound::Sound()
: lastInteractTime(Runner::get().timer.getElapsedTime().asSeconds()) {
    sound.setBuffer(buffer);
}

Runner::Runner()
: paused(false)
, thread(&Runner::run, this)
, shouldStop(false) {
    Runner::started = true;
    BL_LOG_INFO << "Started AudioSystem";
}

Runner::~Runner() { stop(); }

Runner& Runner::get() {
    static Runner runner;
    return runner;
}

void Runner::stop() {
    if (started) {
        BL_LOG_INFO << "Shutting down AudioSystem";
        started = false;
        get().shutdown();
        AudioSystem::stop();
        sf::sleep(sf::milliseconds(500)); // for music threads to stop
        BL_LOG_INFO << "AudioSystem shutdown";
    }
}

void Runner::shutdown() {
    shouldStop = true;
    pauseCond.notify_all();
    thread.join();
}

void Runner::run() {
    while (!shouldStop) {
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
                                unloadTimeout + it->second.buffer.getDuration().asSeconds() &&
                            it->second.sound.getStatus() != sf::Sound::Playing) {
                            sounds.erase(it);
                            soundHandles.erase(j);
                            BL_LOG_INFO << "erased";
                        }
                    }
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
    if (Runner::get().fadeIn.playlist) {
        Runner::get().fadeIn.playlist->pause();
        Runner::get().fadeIn.playlist = nullptr;
    }
    if (in) {
        if (inTime > 0.f) {
            Runner::get().fadeIn.playlist = in;
            Runner::get().fadeIn.fvel     = 100.f / inTime;
            in->setVolume(0.f);
        }
        else {
            in->setVolume(100.f);
        }
        if (!Runner::get().paused) in->play();
    }

    if (Runner::get().fadeOut.playlist) {
        Runner::get().fadeOut.playlist->pause();
        Runner::get().fadeOut.playlist = nullptr;
    }
    if (out) {
        if (outTime > 0.f) {
            Runner::get().fadeOut.playlist = out;
            Runner::get().fadeOut.fvel     = -out->getVolume() / outTime;
        }
        else {
            out->pause();
        }
    }
}

void PlaylistFader::update() {
    if (playlist) {
        const float vol =
            playlist->getVolume() + fvel * Runner::get().fadeTimer.getElapsedTime().asSeconds();
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
    const float v =
        sound.getVolume() + fvel * Runner::get().soundTimer.getElapsedTime().asSeconds();
    if (v <= 0.f) {
        sound.stop();
        Runner::get().fadingSounds.erase(me);
    }
    else if (v >= 100.f) {
        sound.setVolume(100.f);
        Runner::get().fadingSounds.erase(me);
    }
    else {
        sound.setVolume(v);
    }
}

} // namespace
} // namespace audio
} // namespace bl
