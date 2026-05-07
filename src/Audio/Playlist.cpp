#include <BLIB/Audio/Playlist.hpp>

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Serialization.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Random.hpp>

namespace bl
{
namespace audio
{

Playlist::Playlist(as::TypedRef<asi::PlaylistPayload> asset)
: asset(asset)
, _shuffle(asset->getShuffle())
, shuffleOnLoop(asset->getReshuffleOnLoop())
, currentIndex(0)
, playing(false)
, paused(false) {
    playOrder.reserve(asset->getSongCount());
    for (unsigned int i = 0; i < asset->getSongCount(); ++i) { playOrder.push_back(i); }
    if (_shuffle) { shuffle(); }
    openMusic(currentIndex);
}

bool Playlist::isPlaying() const { return playing; }

void Playlist::play() {
    if (!playing && asset->getSongCount() > 0) {
        playing = true;
        if (!paused) {
            if (_shuffle) { shuffle(); }
            currentIndex = 0;
            while (!openMusic(currentIndex)) {
                currentIndex = (currentIndex + 1) % playOrder.size();
                if (currentIndex == 0) {
                    playing = false;
                    break;
                }
            }
            startIndex = currentIndex;
        }
        paused = false;
        if (playing) { current->get().play(); }
    }
}

bool Playlist::openMusic(unsigned int i) {
    if (i != currentIndex && current) {
        current->get().stop();
        current->getAsset().unload();
    }

    current = asset->getSong(playOrder[i]);
    if (!current.getAsset().load()) { return false; }
    return true;
}

void Playlist::pause() {
    playing = false;
    paused  = true;
    current->get().pause();
}

void Playlist::stop() {
    playing = false;
    current->get().stop();
}

void Playlist::setVolume(float volume) { current->get().setVolume(volume); }

float Playlist::getVolume() const { return current->get().getVolume(); }

void Playlist::update() {
    if (playing) {
        if (current->get().getStatus() == sf::Music::Status::Stopped) {
            unsigned int newI = (currentIndex + 1) % playOrder.size();
            if (newI == startIndex && shuffleOnLoop) shuffle();
            while (!openMusic(newI)) {
                newI = (newI + 1) % playOrder.size();
                if (newI == currentIndex) break;
            }
            currentIndex = newI;
            current->get().play();
        }
    }
}

void Playlist::setShuffle(bool s) { _shuffle = s; }

bool Playlist::shuffling() const { return _shuffle; }

void Playlist::setShuffleOnLoop(bool s) { shuffleOnLoop = s; }

bool Playlist::shufflingOnLoop() const { return shuffleOnLoop; }

void Playlist::shuffle() {
    std::vector<unsigned int> priorOrder = playOrder;

    playOrder.clear();
    while (!priorOrder.empty()) {
        const unsigned int i = util::Random::get<unsigned int>(0, priorOrder.size() - 1);
        playOrder.push_back(priorOrder[i]);
        priorOrder.erase(priorOrder.begin() + i);
    }
}

} // namespace audio
} // namespace bl
