#include <BLIB/Media/Audio/AudioSystem.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Util/Random.hpp>
#include <SFML/Audio.hpp>
#include <atomic>
#include <condition_variable>
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
using SoundHandle    = resource::Ref<sf::SoundBuffer>;
using PlaylistHandle = resource::Ref<Playlist>;

struct Sound {
    SoundHandle buffer;
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

    std::unordered_map<AudioSystem::Handle, Sound>::iterator validateAndLoadSound(
        AudioSystem::Handle handle);
    PlaylistHandle validateAndLoadPlaylist(AudioSystem::Handle handle);

    sf::Clock timer;
    float unloadTimeout = 120.f;

    std::unordered_map<std::string, AudioSystem::Handle> soundHandles;
    std::unordered_map<AudioSystem::Handle, std::string> soundSources;
    std::unordered_map<AudioSystem::Handle, Sound> sounds;
    std::list<SoundFader> fadingSounds;
    sf::Clock soundTimer;
    std::shared_mutex soundMutex;

    std::unordered_map<std::string, AudioSystem::Handle> playlistHandles;
    std::unordered_map<AudioSystem::Handle, std::string> playlistSources;
    std::vector<PlaylistHandle> playlistStack;
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

struct SystemSettings {
    const std::string MuteKey   = "blib.audio.muted";
    const std::string VolumeKey = "blib.audio.volume";

    bool muted;
    float volume;

    SystemSettings()
    : muted(false)
    , volume(100.f) {}
} Settings;

std::atomic<bool> Runner::started = false;

inline AudioSystem::Handle makeHandle() {
    auto& s = Runner::get().soundSources;
    AudioSystem::Handle h;
    do {
        h = util::Random::get<AudioSystem::Handle>(1,
                                                   std::numeric_limits<AudioSystem::Handle>::max());
    } while (s.find(h) != s.end());
    return h;
}
} // namespace

void AudioSystem::setUnloadTimeout(float t) { Runner::get().unloadTimeout = t; }

AudioSystem::Handle AudioSystem::getOrLoadSound(const std::string& path) {
    std::unique_lock lock(Runner::get().soundMutex);
    auto& r = Runner::get();

    // Create or find handle
    Handle handle = AudioSystem::InvalidHandle;
    auto it       = r.soundHandles.find(path);
    if (it != r.soundHandles.end()) { handle = it->second; }
    else {
        handle = makeHandle();
        r.soundHandles.emplace(path, handle);
        r.soundSources.emplace(handle, path);
    }

    // Ensure sound loaded
    auto sit = r.sounds.find(handle);
    if (sit == r.sounds.end()) {
        sit                = r.sounds.try_emplace(handle).first;
        sit->second.buffer = resource::ResourceManager<sf::SoundBuffer>::load(path);
        if (sit->second.buffer->getSampleCount() == 0) {
            r.sounds.erase(sit);
            BL_LOG_ERROR << "Failed to load sound: " << path;
            return InvalidHandle;
        }
        sit->second.sound.setBuffer(*sit->second.buffer);
    }

    return handle;
}

bool AudioSystem::playSound(Handle sound, float fadeIn, bool loop) {
    std::shared_lock slock(Runner::get().soundMutex);
    auto& r = Runner::get();
    if (r.paused) return false;

    const auto it = r.validateAndLoadSound(sound);
    if (it == r.sounds.end()) return false;

    // play the sound
    it->second.sound.setLoop(loop);
    it->second.lastInteractTime = r.timer.getElapsedTime().asSeconds();
    if (it->second.sound.getStatus() == sf::Sound::Playing) return true;

    if (fadeIn > 0.f) {
        it->second.sound.setVolume(0.f);
        r.fadingSounds.emplace_back(it->second.sound, 100.f / fadeIn);
        r.fadingSounds.back().me = --r.fadingSounds.end();
    }
    else { it->second.sound.setVolume(100.f); }

    it->second.sound.play();
    return true;
}

bool AudioSystem::playOrRestartSound(Handle sound) {
    std::shared_lock slock(Runner::get().soundMutex);
    auto& r = Runner::get();
    if (r.paused) return false;

    const auto it = r.validateAndLoadSound(sound);
    if (it == r.sounds.end()) return false;

    // play the sound
    it->second.sound.setLoop(false);
    it->second.sound.setVolume(100.f);
    it->second.lastInteractTime = r.timer.getElapsedTime().asSeconds();
    it->second.sound.play();
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

    resource::Ref<Playlist> res = resource::ResourceManager<Playlist>::load(path);
    if (res->getSongList().empty()) return AudioSystem::InvalidHandle;

    const Handle handle = makeHandle();
    Runner::get().playlistHandles.emplace(path, handle);
    Runner::get().playlistSources.emplace(handle, path);
    return handle;
}

bool AudioSystem::pushPlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    PlaylistHandle handle = Runner::get().validateAndLoadPlaylist(playlist);
    if (!handle) return false;

    initiateCrossfade(
        handle.get(),
        Runner::get().playlistStack.empty() ? nullptr : Runner::get().playlistStack.back().get(),
        inTime,
        outTime);
    Runner::get().playlistStack.emplace_back(handle);
    return true;
}

bool AudioSystem::replacePlaylist(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    PlaylistHandle handle = Runner::get().validateAndLoadPlaylist(playlist);
    if (!handle) return false;

    Playlist* out = nullptr;
    if (!Runner::get().playlistStack.empty()) {
        out = Runner::get().playlistStack.back().get();
        // NOTE: If crossfade length exceeds resource timeout this will cause a crash
        Runner::get().playlistStack.pop_back();
    }
    initiateCrossfade(handle.get(), out, inTime, outTime);
    Runner::get().playlistStack.emplace_back(handle);
    return true;
}

void AudioSystem::popPlaylist(float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);
    if (Runner::get().playlistStack.empty()) return;

    Playlist* in  = Runner::get().playlistStack.size() >= 2 ?
                        Runner::get().playlistStack[Runner::get().playlistStack.size() - 2].get() :
                        nullptr;
    Playlist* out = Runner::get().playlistStack.back().get();
    // NOTE: If crossfade length exceeds resource timeout this will cause a crash
    Runner::get().playlistStack.pop_back();
    initiateCrossfade(in, out, inTime, outTime);
}

bool AudioSystem::replaceAllPlaylists(Handle playlist, float inTime, float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    PlaylistHandle handle = Runner::get().validateAndLoadPlaylist(playlist);
    if (!handle) return false;

    initiateCrossfade(
        handle.get(),
        Runner::get().playlistStack.empty() ? nullptr : Runner::get().playlistStack.back().get(),
        inTime,
        outTime);
    // NOTE: If crossfade length exceeds resource timeout this will cause a crash
    Runner::get().playlistStack.clear();
    Runner::get().playlistStack.emplace_back(handle);
    return true;
}

void AudioSystem::stopAllPlaylists(float outTime) {
    std::unique_lock lock(Runner::get().playlistMutex);

    if (!Runner::get().playlistStack.empty()) {
        initiateCrossfade(nullptr, Runner::get().playlistStack.back().get(), 0.f, outTime);
        // NOTE: If crossfade length exceeds resource timeout this will cause a crash
        Runner::get().playlistStack.clear();
    }
}

void AudioSystem::setVolume(float vol) {
    Settings.volume = vol;
    if (!Settings.muted) { sf::Listener::setGlobalVolume(vol); }
}

float AudioSystem::getVolume() { return Settings.volume; }

void AudioSystem::setMuted(bool m) {
    Settings.muted = m;
    sf::Listener::setGlobalVolume(m ? 0.f : Settings.volume);
}

bool AudioSystem::getMuted() { return Settings.muted; }

void AudioSystem::loadFromConfig() {
    Settings.muted = engine::Configuration::getOrDefault<bool>(Settings.MuteKey, Settings.muted);
    Settings.volume =
        engine::Configuration::getOrDefault<float>(Settings.VolumeKey, Settings.volume);
    sf::Listener::setGlobalVolume(Settings.muted ? 0.f : Settings.volume);
}

void AudioSystem::saveToConfig() {
    engine::Configuration::set<bool>(Settings.MuteKey, Settings.muted);
    engine::Configuration::set<float>(Settings.VolumeKey, Settings.volume);
}

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

void AudioSystem::shutdown(bool fade) {
    if (fade) {
        sf::Clock timer;
        while (sf::Listener::getGlobalVolume() > 0.01f) {
            sf::Listener::setGlobalVolume(
                100.f * std::max(0.f, (0.5f - timer.getElapsedTime().asSeconds()) / 0.5f));
            sf::sleep(sf::milliseconds(13));
        }
        sf::Listener::setGlobalVolume(0.f);
    }
    Runner::stop();
}

namespace
{
Sound::Sound()
: lastInteractTime(Runner::get().timer.getElapsedTime().asSeconds()) {}

Runner::Runner()
: paused(false)
, thread(&Runner::run, this)
, shouldStop(false) {
    Runner::started = true;
    BL_LOG_INFO << "Started AudioSystem";
}

Runner::~Runner() {}

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
                                unloadTimeout + it->second.buffer->getDuration().asSeconds() &&
                            it->second.sound.getStatus() != sf::Sound::Playing) {
                            sounds.erase(it);
                            soundHandles.erase(j);
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

std::unordered_map<AudioSystem::Handle, Sound>::iterator Runner::validateAndLoadSound(
    AudioSystem::Handle sound) {
    // load sound if we need to
    auto it = sounds.find(sound);
    if (it == sounds.end()) {
        auto hit = soundSources.find(sound);
        if (hit == soundSources.end()) return sounds.end();
        it                = sounds.try_emplace(sound).first;
        it->second.buffer = resource::ResourceManager<sf::SoundBuffer>::load(hit->second);
        if (it->second.buffer->getSampleCount() == 0) {
            sounds.erase(it);
            return sounds.end();
        }
        it->second.sound.setBuffer(*it->second.buffer);
    }
    return it;
}

PlaylistHandle Runner::validateAndLoadPlaylist(AudioSystem::Handle handle) {
    const auto sit = playlistSources.find(handle);
    if (sit == playlistSources.end()) return {};

    PlaylistHandle res = resource::ResourceManager<Playlist>::load(sit->second);
    if (res->getSongList().empty()) return {};
    return res;
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
        else { in->setVolume(100.f); }
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
        else { out->pause(); }
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
        else { playlist->setVolume(vol); }
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
    else { sound.setVolume(v); }
}

} // namespace
} // namespace audio
} // namespace bl
