#include <BLIB/Audio/AudioSystem.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/System.hpp>
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
struct Sound {
    as::TypedRef<asi::SoundPayload> buffer;
    sf::Sound sound;
    float lastInteractTime;

    Sound(as::TypedRef<asi::SoundPayload>&& buffer);
};

struct SoundFader {
    sf::Sound& sound;
    const float fvel;
    std::list<SoundFader>::iterator me;

    SoundFader(sf::Sound& s, float fvel);
    void update();
};

struct PlaylistFader {
    std::optional<Playlist> localPlaylist;
    Playlist* playlist;
    float fvel;

    PlaylistFader();
    void update();
};

void initiateCrossfade(Playlist* in, Playlist* out, float inTime, float outTime);

class Runner : public engine::System {
public:
    static Runner& get();
    static void stop();

    std::unordered_map<AudioSystem::Handle, Sound>::iterator validateAndLoadSound(
        AudioSystem::Handle handle);
    as::TypedRef<asi::PlaylistPayload> validateAndLoadPlaylist(AudioSystem::Handle handle);

    sf::Clock timer;
    float unloadTimeout = 120.f;

    std::unordered_map<util::UUID, AudioSystem::Handle> soundHandles;
    std::unordered_map<AudioSystem::Handle, util::UUID> soundSources;
    std::unordered_map<AudioSystem::Handle, Sound> sounds;
    std::list<SoundFader> fadingSounds;
    sf::Clock soundTimer;
    std::shared_mutex soundMutex;

    std::unordered_map<util::UUID, AudioSystem::Handle> playlistHandles;
    std::unordered_map<AudioSystem::Handle, util::UUID> playlistSources;
    std::unordered_map<AudioSystem::Handle, as::TypedRef<asi::PlaylistPayload>> playlists;
    std::list<Playlist> playlistStack;
    sf::Clock fadeTimer;
    PlaylistFader fadeIn;
    PlaylistFader fadeOut;
    std::mutex playlistMutex;

    std::mutex pauseMutex;
    std::condition_variable pauseCond;
    bool paused;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex&, float, float, float, float) override;
    virtual void earlyCleanup() override;

    Runner();
    ~Runner();

    static Runner* instance;
};

Runner* Runner::instance = nullptr;

struct SystemSettings {
    const std::string MuteKey   = "blib.audio.muted";
    const std::string VolumeKey = "blib.audio.volume";

    bool muted;
    float volume;

    SystemSettings()
    : muted(false)
    , volume(100.f) {}
} Settings;

AudioSystem::Handle makeHandle() {
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

AudioSystem::Handle AudioSystem::getOrLoadSound(as::TypedRef<asi::SoundPayload> sound) {
    if (!sound) {
        BL_LOG_ERROR << "Attempted to load invalid sound";
        return InvalidHandle;
    }
    if (sound->getAsset().getState() != as::State::Loaded) {
        BL_LOG_INFO << "Sound asset '" << sound.getUUID() << "' is not loaded, attempting to load";
        if (!sound->getAsset().load()) {
            BL_LOG_ERROR << "Failed to load sound asset";
            return InvalidHandle;
        }
    }

    std::unique_lock lock(Runner::get().soundMutex);
    auto& r = Runner::get();

    // Create or find handle
    Handle handle = AudioSystem::InvalidHandle;
    auto it       = r.soundHandles.find(sound.getUUID());
    if (it != r.soundHandles.end()) { handle = it->second; }
    else {
        handle = makeHandle();
        r.soundHandles.try_emplace(sound.getUUID(), handle);
        r.soundSources.try_emplace(handle, sound.getUUID());
        r.sounds.try_emplace(handle, std::move(sound));
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
    it->second.sound.setLooping(loop);
    it->second.lastInteractTime = r.timer.getElapsedTime().asSeconds();
    if (it->second.sound.getStatus() == sf::Sound::Status::Playing) return true;

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
    it->second.sound.setLooping(false);
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
        if (it->second.sound.getStatus() == sf::Sound::Status::Playing) {
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
        if (sound.second.sound.getStatus() == sf::Sound::Status::Playing) {
            sound.second.sound.stop();
        }
    }
}

AudioSystem::Handle AudioSystem::getOrLoadPlaylist(as::TypedRef<asi::PlaylistPayload> asset) {
    if (!asset) {
        BL_LOG_ERROR << "Attempted to load invalid playlist";
        return InvalidHandle;
    }
    if (asset->getAsset().getState() != as::State::Loaded) {
        BL_LOG_INFO << "Playlist asset '" << asset.getUUID()
                    << "' is not loaded. Attempting to load";
        if (!asset->getAsset().load()) {
            BL_LOG_ERROR << "Failed to load playlist asset";
            return InvalidHandle;
        }
    }
    if (asset->getSongCount() == 0) {
        BL_LOG_ERROR << "Playlist asset '" << asset.getUUID() << "' is empty";
        return InvalidHandle;
    }

    auto& r = Runner::get();
    std::unique_lock lock(r.playlistMutex);

    const auto it = r.playlistHandles.find(asset.getUUID());
    if (it != r.playlistHandles.end()) return it->second;

    const Handle handle = makeHandle();
    r.playlistHandles.try_emplace(asset.getUUID(), handle);
    r.playlistSources.try_emplace(handle, asset.getUUID());
    r.playlists.try_emplace(handle, std::move(asset));
    return handle;
}

bool AudioSystem::pushPlaylist(Handle playlist, float inTime, float outTime) {
    auto& r = Runner::get();
    std::unique_lock lock(r.playlistMutex);

    auto handle = r.validateAndLoadPlaylist(playlist);
    if (!handle) return false;

    auto* current = r.playlistStack.empty() ? nullptr : &r.playlistStack.back();
    auto& next    = r.playlistStack.emplace_back(handle);
    initiateCrossfade(&next, current, inTime, outTime);
    return true;
}

bool AudioSystem::replacePlaylist(Handle playlist, float inTime, float outTime) {
    auto& r = Runner::get();
    std::unique_lock lock(r.playlistMutex);

    auto handle = r.validateAndLoadPlaylist(playlist);
    if (!handle) return false;

    Playlist* out = nullptr;
    if (!r.playlistStack.empty()) {
        r.fadeOut.localPlaylist.emplace(r.playlistStack.back());
        out = &r.fadeOut.localPlaylist.value();
        r.playlistStack.pop_back();
    }
    Playlist& next = r.playlistStack.emplace_back(handle);
    initiateCrossfade(&next, out, inTime, outTime);
    return true;
}

void AudioSystem::popPlaylist(float inTime, float outTime) {
    auto& r = Runner::get();
    std::unique_lock lock(r.playlistMutex);
    if (r.playlistStack.empty()) return;

    Playlist* in = r.playlistStack.size() >= 2 ? &*(++r.playlistStack.rbegin()) : nullptr;
    r.fadeOut.localPlaylist.emplace(r.playlistStack.back());
    Playlist* out = &r.fadeOut.localPlaylist.value();
    r.playlistStack.pop_back();
    initiateCrossfade(in, out, inTime, outTime);
}

bool AudioSystem::replaceAllPlaylists(Handle playlist, float inTime, float outTime) {
    auto& r = Runner::get();
    std::unique_lock lock(r.playlistMutex);

    auto handle = r.validateAndLoadPlaylist(playlist);
    if (!handle) return false;

    Playlist* out = nullptr;
    if (!r.playlistStack.empty()) {
        r.fadeOut.localPlaylist.emplace(r.playlistStack.back());
        out = &r.fadeOut.localPlaylist.value();
    }

    r.playlistStack.clear();
    auto& next = r.playlistStack.emplace_back(handle);

    initiateCrossfade(&next, out, inTime, outTime);
    return true;
}

void AudioSystem::stopAllPlaylists(float outTime) {
    auto& r = Runner::get();
    std::unique_lock lock(r.playlistMutex);

    if (!r.playlistStack.empty()) {
        r.fadeOut.localPlaylist.emplace(r.playlistStack.back());
        Playlist* out = &r.fadeOut.localPlaylist.value();
        initiateCrossfade(nullptr, out, 0.f, outTime);
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
    auto& r = Runner::get();
    std::unique_lock lock(r.pauseMutex);
    if (r.paused) return;
    r.paused = true;

    for (auto& sound : r.sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Status::Playing) {
            sound.second.sound.pause();
        }
    }
    if (!r.playlistStack.empty() && r.playlistStack.back().isPlaying()) {
        r.playlistStack.back().pause();
    }
    if (r.fadeIn.playlist && r.fadeIn.playlist->isPlaying()) { r.fadeIn.playlist->pause(); }
    if (r.fadeOut.playlist && r.fadeOut.playlist->isPlaying()) { r.fadeOut.playlist->pause(); }
}

void AudioSystem::resume() {
    auto& r = Runner::get();
    std::unique_lock lock(r.pauseMutex);
    if (!r.paused) return;
    r.paused = false;

    for (auto& sound : r.sounds) {
        if (sound.second.sound.getStatus() == sf::Sound::Status::Paused) {
            sound.second.sound.play();
        }
    }
    if (!r.playlistStack.empty()) { r.playlistStack.back().play(); }
    if (r.fadeIn.playlist) { r.fadeIn.playlist->play(); }
    if (r.fadeOut.playlist) { r.fadeOut.playlist->play(); }

    r.pauseCond.notify_all();
}

void AudioSystem::stop(bool fade) {
    if (fade) {
        sf::Clock timer;
        while (sf::Listener::getGlobalVolume() > 0.01f) {
            sf::Listener::setGlobalVolume(
                100.f * std::max(0.f, (0.5f - timer.getElapsedTime().asSeconds()) / 0.5f));
            sf::sleep(sf::milliseconds(13));
        }
        sf::Listener::setGlobalVolume(0.f);
    }

    auto& r = Runner::get();
    {
        std::unique_lock lock(r.soundMutex);
        for (auto& sound : r.sounds) { sound.second.sound.stop(); }
    }

    {
        std::unique_lock lock(r.playlistMutex);
        for (auto p : r.playlistStack) { p.stop(); }
        r.playlistStack.clear();
        r.fadeIn.localPlaylist.reset();
        r.fadeOut.localPlaylist.reset();
        r.fadeIn.playlist  = nullptr;
        r.fadeOut.playlist = nullptr;
    }

    if (fade) { sf::Listener::setGlobalVolume(1.f); }
}

void AudioSystem::registerSystem(engine::Engine& engine) {
    engine.systems().registerBackgroundSystem<Runner>(engine::FrameStage::FrameStart,
                                                      engine::StateMask::All);
}

namespace
{
Sound::Sound(as::TypedRef<asi::SoundPayload>&& buffer)
: buffer(std::move(buffer))
, sound(this->buffer->get())
, lastInteractTime(Runner::get().timer.getElapsedTime().asSeconds()) {}

Runner::Runner()
: paused(false) {}

Runner::~Runner() {
    if (instance == this) { instance = nullptr; }
}

Runner& Runner::get() {
    if (!instance) { BL_LOG_CRITICAL << "Accessing AudioSystem before it is initialized!"; }

    return *instance;
}

void Runner::init(engine::Engine&) {
    if (instance) {
        BL_LOG_CRITICAL << "AudioSystem initialized more than once!";
        return;
    }
    instance = this;
    AudioSystem::loadFromConfig();
}

void Runner::earlyCleanup() {
    BL_LOG_INFO << "Shutting down AudioSystem";
    AudioSystem::stop(true);
    sf::sleep(sf::milliseconds(500)); // for music threads to stop
    BL_LOG_INFO << "AudioSystem shutdown";
}

void Runner::update(std::mutex&, float, float, float, float) {
    if (paused) { return; }

    // sound cleanup and fadeouts
    {
        std::unique_lock slock(soundMutex);

        for (auto i = soundHandles.begin(); i != soundHandles.end();) {
            auto j  = i++;
            auto it = sounds.find(j->second);
            if (it != sounds.end()) {
                if (timer.getElapsedTime().asSeconds() - it->second.lastInteractTime >=
                        unloadTimeout + it->second.buffer->get().getDuration().asSeconds() &&
                    it->second.sound.getStatus() != sf::Sound::Status::Playing) {
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

        if (!playlistStack.empty() && playlistStack.front().isPlaying()) {
            playlistStack.front().update();
        }

        fadeIn.update();
        fadeOut.update();
        fadeTimer.restart();
    }
}

std::unordered_map<AudioSystem::Handle, Sound>::iterator Runner::validateAndLoadSound(
    AudioSystem::Handle sound) {
    return sounds.find(sound);
}

as::TypedRef<asi::PlaylistPayload> Runner::validateAndLoadPlaylist(AudioSystem::Handle handle) {
    const auto sit = playlists.find(handle);
    if (sit == playlists.end()) return {};
    return sit->second;
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

PlaylistFader::PlaylistFader()
: playlist(nullptr) {}

void PlaylistFader::update() {
    if (playlist) {
        const float vol =
            playlist->getVolume() + fvel * Runner::get().fadeTimer.getElapsedTime().asSeconds();
        if (vol <= 0.f) {
            playlist->setVolume(0.f);
            playlist->pause();
            localPlaylist.reset();
            playlist = nullptr;
        }
        else if (vol >= 100.f) {
            playlist->setVolume(100.f);
            localPlaylist.reset();
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
