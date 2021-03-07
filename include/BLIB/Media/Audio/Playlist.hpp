#ifndef BLIB_MEDIA_AUDIO_PLAYLIST_HPP
#define BLIB_MEDIA_AUDIO_PLAYLIST_HPP

#include <BLIB/Files/Binary.hpp>
#include <SFML/Audio/Music.hpp>

#include <string>
#include <vector>

namespace bl
{
class Playlist : public bf::SerializableObject {
public:
    Playlist();

    Playlist(const std::string& source);

    Playlist(const Playlist&);

    Playlist& operator=(const Playlist& copy);

    bool isPlaying() const;

    void play();

    void pause();

    void stop();

    void update();

    void setVolume(float volume);

    void addSong(const std::string& song);

    void removeSong(const std::string& song);

    const std::vector<std::string>& getSongList() const;

    void setShuffle(bool shuffle);

    bool shuffling() const;

private:
    bf::SerializableField<1, std::vector<std::string>> songs;
    bf::SerializableField<2, bool> _shuffle;

    sf::Music current;
    unsigned int currentIndex;
    bool playing, paused;

    void shuffle();
};

} // namespace bl

#endif
