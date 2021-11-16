#include <BLIB/Logging.hpp>
#include <BLIB/Media/Audio/AudioSystem.hpp>
#include <BLIB/Util/Random.hpp>

#include <chrono>
#include <limits>

namespace bl
{
namespace audio
{
namespace
{
constexpr unsigned int UpdatePeriod    = 100;  // milliseconds
constexpr float FadeAmount             = 0.1f; // 10hz updates = 1 second fade
constexpr float FadeTolerance          = 0.001f;
constexpr float DefaultMinFadeDistance = 64.f;
constexpr float DefaultAttenuation     = 1.f;
constexpr float DefaultMaxDistance     = 320.f * 320.f;
constexpr float SoundCapacityFactor    = 0.2f; // 20% size:capacity ratio

} // namespace

AudioSystem::AudioSystem()
: masterVolume(100.f)
, runner(&AudioSystem::background, this)
, state(SystemState::Running)
, fadeVolumeFactor(-1.f)
, musicState(MusicState::Stopped)
, musicVolumeFactor(1)
, musicFadeAmount(0)
, defaultSpatialSettings{DefaultMinFadeDistance, DefaultAttenuation}
, maxSpatialDistanceSquared(DefaultMaxDistance) {
    sf::Listener::setUpVector(0, 0, -1); // TODO - correct?
    BL_LOG_INFO << "AudioSystem online";
}

AudioSystem::~AudioSystem() {
    BL_LOG_INFO << "Audiosystem shutting down";
    stopAll();
    state = SystemState::Stopping;
    pauseSync.notify_all();
    runner.join();
    BL_LOG_INFO << "AudioSystem shutdown";
}

AudioSystem& AudioSystem::get() {
    static AudioSystem system;
    return system;
}

void AudioSystem::setVolumeImp(float v) { masterVolume = v; }

void AudioSystem::stopAllImp(bool fade) {
    fadeVolumeFactor = (fade && state != SystemState::Paused) ? 1.f : 0.0001f;
}

void AudioSystem::pauseImp() {
    std::unique_lock lock(pauseMutex);
    state = SystemState::Paused;
}

void AudioSystem::resumeImp() {
    if (state == SystemState::Paused) {
        state = SystemState::Running;
        pauseSync.notify_all();
    }
}

void AudioSystem::setListenerPositionImp(const sf::Vector2f& pos) {
    sf::Listener::setPosition({pos.x, pos.y, 0});
}

void AudioSystem::setDefaultSpatialSoundSettingsImp(const SpatialSettings& settings) {
    std::unique_lock lock(soundMutex);
    defaultSpatialSettings = settings;
}

void AudioSystem::setSpatialSoundCutoffDistanceImp(float md) {
    std::unique_lock lock(soundMutex);
    maxSpatialDistanceSquared = md;
}

void AudioSystem::pushPlaylistImp(const Playlist& np, float fadeout) {
    std::unique_lock lock(playlistMutex);

    playlists.emplace_back(new Playlist(np));
    musicVolumeFactor = 1.f;
    musicFadeAmount   = 1.f / (fadeout * 10.f);
    if (playlists.size() == 1) {
        playlists.back()->setVolume(volume());
        musicState = MusicState::Playing;
    }
    else {
        musicState = MusicState::Pushing;
        playlists.back()->setVolume(0);
    }
    if (state != SystemState::Paused) playlists.back()->play();
}

void AudioSystem::replacePlaylistImp(const Playlist& np, float fadeout) {
    std::unique_lock lock(playlistMutex);

    playlists.emplace_back(new Playlist(np));
    musicVolumeFactor = 1.f;
    musicFadeAmount   = 1.f / (fadeout * 10.f);
    if (playlists.size() == 1) {
        playlists.back()->setVolume(volume());
        musicState = MusicState::Playing;
    }
    else {
        musicState = MusicState::Replacing;
        playlists.back()->setVolume(0);
    }
    if (state != SystemState::Paused) playlists.back()->play();
}

void AudioSystem::popPlaylistImp(float fadeout) {
    if (!playlists.empty()) {
        std::unique_lock lock(playlistMutex);
        musicState        = MusicState::Popping;
        musicVolumeFactor = 1.f;
        musicFadeAmount   = 1.f / (fadeout * 10.f);
    }
}

AudioSystem::Handle AudioSystem::create() const {
    return util::Random::get<Handle>(1, std::numeric_limits<Handle>::max());
}

AudioSystem::Handle AudioSystem::playSoundImp(resource::Resource<sf::SoundBuffer>::Ref sound,
                                              bool loop) {
    const Handle h = create();
    std::unique_lock lock(soundMutex);
    auto s = *sounds.emplace(new Sound(h, sound));
    s->sound.setVolume(volume());
    s->sound.setLoop(loop);
    if (state != SystemState::Paused) s->sound.play();
    soundMap.emplace(h, s);
    return h;
}

AudioSystem::Handle AudioSystem::playSpatialSoundImp(resource::Resource<sf::SoundBuffer>::Ref sound,
                                                     const sf::Vector2f& pos, bool loop) {
    return playSpatialSound(sound, pos, defaultSpatialSettings, loop);
}

AudioSystem::Handle AudioSystem::playSpatialSoundImp(resource::Resource<sf::SoundBuffer>::Ref sound,
                                                     const sf::Vector2f& pos,
                                                     const SpatialSettings& settings, bool loop) {
    std::unique_lock lock(soundMutex);
    const float dx = pos.x - sf::Listener::getPosition().x;
    const float dy = pos.y - sf::Listener::getPosition().y;
    if (dx * dx + dy * dy >= maxSpatialDistanceSquared) return InvalidHandle;

    const Handle h = create();
    auto s         = *sounds.emplace(new Sound(h, sound));
    s->sound.setVolume(volume());
    s->sound.setPosition({pos.x, pos.y, 0});
    s->sound.setMinDistance(settings.fadeStartDistance);
    s->sound.setAttenuation(settings.attenuation);
    s->sound.setLoop(loop);
    if (state != SystemState::Paused) s->sound.play();
    soundMap.emplace(h, s);
    return h;
}

std::shared_ptr<AudioSystem::Sound> AudioSystem::getSoundImp(Handle h) {
    std::shared_lock lock(soundMutex);
    auto it = soundMap.find(h);
    if (it == soundMap.end()) return nullptr;
    return it->second;
}

void AudioSystem::stopSoundImp(Handle h) {
    std::unique_lock lock(soundMutex);
    auto it = soundMap.find(h);
    if (it == soundMap.end()) return;
    it->second->sound.stop();
}

void AudioSystem::stopSoundImp(Handle h, float fadeOut) {
    std::unique_lock lock(soundMutex);
    auto it = soundMap.find(h);
    if (it == soundMap.end()) return;
    it->second->fadeOut    = fadeOut;
    it->second->fadeFactor = 1.f;
}

float AudioSystem::volume() const {
    if (fadeVolumeFactor < 0) return masterVolume;
    return masterVolume * fadeVolumeFactor;
}

void AudioSystem::background() {
    const auto crossfade = [this](bool lvolUp) {
        musicVolumeFactor -= musicFadeAmount;
        const float f  = std::max(0.f, musicVolumeFactor);
        const float lf = lvolUp ? (1.f - f) : f;
        const float pf = !lvolUp ? (1.f - f) : f;
        playlists.back()->setVolume(volume() * lf);
        playlists.back()->update();
        playlists.back()->play();
        if (playlists.size() > 1) { // cross fade
            playlists[playlists.size() - 2]->update();
            playlists[playlists.size() - 2]->play();
            playlists[playlists.size() - 2]->setVolume(volume() * pf);
        }
    };

    const auto finishCrossfade = [this]() {
        musicVolumeFactor = 1.f;
        if (!playlists.empty()) {
            playlists.back()->setVolume(volume());
            if (state != SystemState::Paused) playlists.back()->play();
        }
        musicState = playlists.empty() ? MusicState::Stopped : MusicState::Playing;
    };

    sf::Clock soundFadeTimer;
    while (state != SystemState::Stopping || fadeVolumeFactor > 0.f) {
        {
            // sleep if paused
            std::unique_lock lock(pauseMutex);
            if (state == SystemState::Paused) {
                {
                    std::unique_lock mlock(playlistMutex);
                    std::unique_lock slock(soundMutex);
                    for (auto& s : playlists) { s->pause(); }
                    for (auto& s : sounds) { s->sound.pause(); }
                }
                pauseSync.wait(lock);

                // unpaused
                std::unique_lock mlock(playlistMutex);
                std::unique_lock slock(soundMutex);
                for (auto& s : sounds) {
                    if (s->sound.getStatus() == sf::Sound::Paused) s->sound.play();
                }
                if (playlists.size() > 2) {
                    for (unsigned int i = 0; i < playlists.size(); ++i) { playlists[i]->stop(); }
                }
                if (!playlists.empty()) {
                    playlists.back()->play();
                    if (musicState != MusicState::Playing && playlists.size() > 1) {
                        playlists[playlists.size() - 1]->play(); // resume crossfade
                    }
                }
            }

            // Update fadeout
            if (fadeVolumeFactor > 0.f) {
                if (fadeVolumeFactor <= FadeTolerance) { // done fading out
                    std::unique_lock mlock(playlistMutex);
                    std::unique_lock slock(soundMutex);

                    sounds.clear();
                    soundMap.clear();
                    playlists.clear();
                    musicState       = MusicState::Stopped;
                    fadeVolumeFactor = -1.f;
                }
                else {
                    // volumes are updated below
                    fadeVolumeFactor =
                        std::max(fadeVolumeFactor - FadeAmount, FadeTolerance / 10.f);
                }
            }

            // Update playlists
            {
                std::unique_lock lock(playlistMutex);

                switch (musicState) {
                case MusicState::Popping:
                    crossfade(false);
                    if (musicVolumeFactor <= 0) { // fade complete
                        playlists.pop_back();
                        finishCrossfade();
                    }
                    break;

                case MusicState::Replacing:
                case MusicState::Pushing:
                    crossfade(true);
                    if (musicVolumeFactor <= 0) {   // fade complete
                        if (playlists.size() > 1) { // stop old one
                            playlists[playlists.size() - 2]->stop();
                            if (musicState == MusicState::Replacing) // erase replaced list
                                playlists.erase(playlists.begin() + playlists.size() - 2);
                        }
                        finishCrossfade();
                    }
                    break;

                case MusicState::Playing:
                    playlists.back()->setVolume(volume());
                    playlists.back()->update();
                    break;

                default:
                    break;
                }
            }

            // update sounds
            {
                std::unique_lock lock(soundMutex);

                const float v = volume();
                for (auto it = sounds.begin(); it != sounds.end(); ++it) {
                    if (it->get()->sound.getStatus() == sf::Sound::Stopped) {
                        soundMap.erase(it->get()->handle);
                        sounds.erase(it); // only partially invalidated, can still increment
                    }
                    else if (it->get()->fadeOut > 0.f) {
                        // fade out then stop and remove
                        it->get()->fadeFactor -=
                            soundFadeTimer.getElapsedTime().asSeconds() / it->get()->fadeOut;
                        if (it->get()->fadeFactor > 0.f) {
                            it->get()->sound.setVolume(v * it->get()->fadeFactor);
                        }
                        else {
                            soundMap.erase(it->get()->handle);
                            sounds.erase(it); // only partially invalidated, can still increment
                        }
                    }
                    else {
                        it->get()->sound.setVolume(v);
                    }
                }

                const float loadFactor =
                    static_cast<float>(sounds.size()) / static_cast<float>(sounds.capacity());
                if (loadFactor <= SoundCapacityFactor) { sounds.shrink(); }
            }
        }

        // sleep
        soundFadeTimer.restart();
        std::this_thread::sleep_for(std::chrono::milliseconds(UpdatePeriod));
    }
}

AudioSystem::Sound::Sound(AudioSystem::Handle h, resource::Resource<sf::SoundBuffer>::Ref r)
: handle(h)
, buffer(r)
, sound(*r)
, fadeOut(-1.f)
, fadeFactor(-1.f) {}

} // namespace audio
} // namespace bl
