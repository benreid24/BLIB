#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Files/FileUtil.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Media/Audio/Playlist.hpp>
#include <BLIB/Util/Random.hpp>

namespace bl
{
namespace
{
inline std::string songfile(const std::string& path) {
    return FileUtil::joinPath(engine::Configuration::get<std::string>("blib.playlist.song_path"),
                              path);
}
} // namespace

Playlist::Playlist()
: songs(*this)
, _shuffle(*this, false)
, shuffleOnLoop(*this, false)
, playing(false)
, paused(false)
, currentIndex(0) {}

Playlist::Playlist(const std::string& file)
: Playlist() {
    bf::BinaryFile input(file, bf::BinaryFile::Read);
    if (!deserialize(input)) { BL_LOG_ERROR << "Failed to load playlist from file: " << file; }
}

Playlist::Playlist(const Playlist& copy)
: Playlist() {
    songs = copy.songs.getValue();
    _shuffle.setValue(copy._shuffle);
    shuffleOnLoop.setValue(copy.shuffleOnLoop);
}

Playlist& Playlist::operator=(const Playlist& copy) {
    songs = copy.songs.getValue();
    _shuffle.setValue(copy._shuffle);
    shuffleOnLoop.setValue(copy.shuffleOnLoop);
    return *this;
}

bool Playlist::isPlaying() const { return playing; }

void Playlist::play() {
    if (!playing && !songs.getValue().empty()) {
        playing = true;
        if (!paused) {
            if (_shuffle) shuffle();
            currentIndex = 0;
            while (!current.openFromFile(songfile(songs.getValue()[currentIndex]))) {
                currentIndex = (currentIndex + 1) % songs.getValue().size();
                if (currentIndex == 0) {
                    playing = false;
                    break;
                }
            }
            startIndex = currentIndex;
        }
        paused = false;
        if (playing) current.play();
    }
}

void Playlist::pause() {
    playing = false;
    paused  = true;
    current.pause();
}

void Playlist::stop() {
    playing = false;
    current.stop();
}

void Playlist::setVolume(float volume) { current.setVolume(volume); }

void Playlist::update() {
    if (playing) {
        if (current.getStatus() == sf::Music::Stopped) {
            unsigned int newI = (currentIndex + 1) % songs.getValue().size();
            if (newI == startIndex && shuffleOnLoop) shuffle();
            while (!current.openFromFile(songfile(songs.getValue()[newI]))) {
                newI = (newI + 1) % songs.getValue().size();
                if (newI == currentIndex) break;
            }
            currentIndex = newI;
            current.play();
        }
    }
}

void Playlist::addSong(const std::string& song) { songs.getValue().push_back(song); }

void Playlist::removeSong(const std::string& song) {
    for (unsigned int i = 0; i < songs.getValue().size(); ++i) {
        if (songs.getValue()[i] == song) {
            songs.getValue().erase(songs.getValue().begin() + i);
            --i;
        }
    }
}

const std::vector<std::string>& Playlist::getSongList() const { return songs.getValue(); }

void Playlist::setShuffle(bool s) { _shuffle = s; }

bool Playlist::shuffling() const { return _shuffle; }

void Playlist::setShuffleOnLoop(bool s) { shuffleOnLoop = s; }

bool Playlist::shufflingOnLoop() const { return shuffleOnLoop; }

void Playlist::shuffle() {
    const std::vector<std::string> order(songs.getMovable());
    FastEraseVector<std::size_t> indices;
    indices.reserve(songs.getValue().size());
    songs.getValue().reserve(order.size());

    for (std::size_t i = 0; i < order.size(); ++i) { indices.push_back(i); }

    while (!indices.empty()) {
        const std::size_t i = util::Random::get<std::size_t>(0, indices.size() - 1);
        songs.getValue().emplace_back(std::move(order[indices[i]]));
        indices.erase(i);
    }
}

} // namespace bl
