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

    Playlist(const Playlist& copy);

    Playlist(Playlist&& copy);

    Playlist& operator=(Playlist&& copy);

    bool isPlaying() const;

    void play();

    void pause();

    void resume();

    void stop();

    void update();

    void addSong(const std::string& song);

    void removeSong(const std::string& song);

    const std::vector<std::string>& getSongList() const;

private:
    bf::SerializableField<1, std::vector<std::string>> songs;
    bf::SerializableField<2, bool> _shuffle;

    sf::Music current;
    unsigned int currentIndex;
    bool playing;

    void shuffle();
};

} // namespace bl

#endif
