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
namespace
{
inline std::string songfile(const std::string& path) {
    return util::FileUtil::joinPath(
        engine::Configuration::get<std::string>("blib.playlist.song_path"), path);
}

using BinarySerializer = serial::binary::Serializer<Playlist>;
using JsonSerializer   = serial::json::Serializer<Playlist>;
} // namespace

Playlist::Playlist()
: _shuffle(false)
, shuffleOnLoop(false)
, currentIndex(0)
, playing(false)
, paused(false) {}

Playlist::Playlist(const std::string& file)
: Playlist() {
    loadFromFile(file);
}

Playlist::Playlist(const Playlist& copy)
: Playlist() {
    songs         = copy.songs;
    _shuffle      = copy._shuffle;
    shuffleOnLoop = copy.shuffleOnLoop;
}

Playlist::~Playlist() {
    if (!songs.empty() && playing) {
        stop();
        resource::FileSystem::purgePersistentData(songfile(songs[currentIndex]));
    }
}

bool Playlist::saveToFile(const std::string& path) const {
    std::ofstream file(path.c_str());
    return JsonSerializer::serializeStream(file, *this, 4, 0);
}

bool Playlist::saveToMemory(serial::binary::OutputStream& os) const {
    return serial::binary::Serializer<Playlist>::serialize(os, *this);
}

bool Playlist::loadFromFile(const std::string& path) {
    std::ifstream file(path.c_str());
    if (!JsonSerializer::deserializeStream(file, *this)) {
        BL_LOG_ERROR << "Failed to load playlist from file: " << path;
        return false;
    }
    playing      = false;
    paused       = false;
    currentIndex = 0;
    return true;
}

bool Playlist::loadBinary(const char* buffer, std::size_t len) {
    serial::MemoryInputBuffer buf(buffer, len);
    serial::binary::InputStream in(buf);
    if (!BinarySerializer::deserialize(in, *this)) {
        BL_LOG_ERROR << "Failed to load playlist from memory";
        return false;
    }
    playing      = false;
    paused       = false;
    currentIndex = 0;
    return true;
}

bool Playlist::loadJson(const char* buffer, std::size_t len) {
    util::BufferIstreamBuf buf(const_cast<char*>(buffer), len);
    std::istream is(&buf);
    if (!JsonSerializer::deserializeStream(is, *this)) {
        BL_LOG_ERROR << "Failed to load playlist from memory";
        return false;
    }
    playing      = false;
    paused       = false;
    currentIndex = 0;
    return true;
}

Playlist& Playlist::operator=(const Playlist& copy) {
    songs         = copy.songs;
    _shuffle      = copy._shuffle;
    shuffleOnLoop = copy.shuffleOnLoop;
    return *this;
}

bool Playlist::isPlaying() const { return playing; }

void Playlist::play() {
    if (!playing && !songs.empty()) {
        playing = true;
        if (!paused) {
            if (_shuffle) shuffle();
            currentIndex = 0;
            while (!openMusic(currentIndex)) {
                currentIndex = (currentIndex + 1) % songs.size();
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

bool Playlist::openMusic(unsigned int i) {
    if (i != currentIndex) {
        current.stop();
        resource::FileSystem::purgePersistentData(songfile(songs[currentIndex]));
    }
    char* buffer    = nullptr;
    std::size_t len = 0;
    if (!resource::FileSystem::getData(songfile(songs[i]), &buffer, len)) return false;
    return current.openFromMemory(buffer, len);
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

float Playlist::getVolume() const { return current.getVolume(); }

void Playlist::update() {
    if (playing) {
        if (current.getStatus() == sf::Music::Stopped) {
            unsigned int newI = (currentIndex + 1) % songs.size();
            if (newI == startIndex && shuffleOnLoop) shuffle();
            while (!openMusic(newI)) {
                newI = (newI + 1) % songs.size();
                if (newI == currentIndex) break;
            }
            currentIndex = newI;
            current.play();
        }
    }
}

void Playlist::addSong(const std::string& song) { songs.push_back(song); }

void Playlist::removeSong(const std::string& song) {
    for (unsigned int i = 0; i < songs.size(); ++i) {
        if (songs[i] == song) {
            songs.erase(songs.begin() + i);
            --i;
        }
    }
}

const std::vector<std::string>& Playlist::getSongList() const { return songs; }

void Playlist::setShuffle(bool s) { _shuffle = s; }

bool Playlist::shuffling() const { return _shuffle; }

void Playlist::setShuffleOnLoop(bool s) { shuffleOnLoop = s; }

bool Playlist::shufflingOnLoop() const { return shuffleOnLoop; }

void Playlist::shuffle() {
    const std::vector<std::string> order(std::move(songs));
    ctr::FastEraseVector<std::size_t> indices;
    indices.reserve(songs.size());
    songs.reserve(order.size());
    songs.clear();

    for (std::size_t i = 0; i < order.size(); ++i) { indices.push_back(i); }

    while (!indices.empty()) {
        const std::size_t i = util::Random::get<std::size_t>(0, indices.size() - 1);
        songs.emplace_back(std::move(order[indices[i]]));
        indices.erase(i);
    }
}

} // namespace audio
} // namespace bl
