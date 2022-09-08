#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Media/Audio.hpp>
#include <BLIB/Serialization/Binary.hpp>

#include <chrono>
#include <iostream>
#include <thread>

using bl::audio::AudioSystem;

int main() {
    // We will use these later for demonstrating AudioSystem
    bl::audio::Playlist list1;
    list1.addSong("resources/song1.ogg");
    list1.save("list1.plst");
    bl::audio::Playlist list2;
    list2.addSong("resources/song2.ogg");
    list2.save("list2.plst");
    bl::audio::Playlist list3;
    list3.addSong("resources/song3.ogg");
    list3.save("list3.plst");

    // playlists may be created manually
    bl::audio::Playlist playlist;
    playlist.addSong("resources/hardRain.wav");
    playlist.addSong("resources/lightRain.wav");
    playlist.addSong("resources/foundItem.wav");
    playlist.setShuffle(true);
    playlist.play();

    // if using directly update() must be called periodically
    std::cout << "Playing playlist manually\n";
    unsigned int ttime = 0;
    while (ttime <= 45) {
        playlist.update();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ++ttime;
    }
    playlist.stop();

    // AudioSystem has a background thread which calls update() for us
    AudioSystem::Handle h1 = AudioSystem::getOrLoadPlaylist("list1.plst");
    AudioSystem::pushPlaylist(h1);
    std::cout << "Playing playlist through AudioSystem\n";

    // AudioSystem also manages running sounds
    AudioSystem::Handle thunder = AudioSystem::getOrLoadSound("resources/thunder.wav");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Playing thunder\n";
    AudioSystem::playSound(thunder);

    // Sounds may be looped
    AudioSystem::Handle save = AudioSystem::getOrLoadSound("resources/gameSave.wav");
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "Looping eagle\n";
    AudioSystem::playSound(save, -1.f, true);

    // Everything may be paused
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "Pausing\n";
    AudioSystem::pause();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Unpausing\n";
    AudioSystem::resume();

    // Sounds may be stopped using their handles
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "Stopping eagle\n";
    AudioSystem::stopSound(save);

    // AudioSystem maintains a stack of playlists
    AudioSystem::Handle p2 = AudioSystem::getOrLoadPlaylist("list2.plst");
    AudioSystem::pushPlaylist(p2);
    std::cout << "Pushing new playlist\n";
    std::this_thread::sleep_for(std::chrono::seconds(7));
    std::cout << "Popping it\n";
    AudioSystem::popPlaylist();

    // Playlists may also be replaced
    AudioSystem::Handle p3 = AudioSystem::getOrLoadPlaylist("list3.plst");
    std::this_thread::sleep_for(std::chrono::seconds(7));
    std::cout << "Replacing current playlist\n";
    AudioSystem::replacePlaylist(p3); // replace current playing with mylist
    std::this_thread::sleep_for(std::chrono::seconds(7));

    // AudioSystem fade out and clean up
    AudioSystem::shutdown();

    return 0;
}
