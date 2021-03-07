#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Media/Audio.hpp>

#include <chrono>
#include <iostream>
#include <thread>

int main() {
    // We will use these later for demonstrating AudioSystem
    bl::Playlist list1;
    list1.addSong("resources/song1.ogg");
    bl::Playlist list2;
    list2.addSong("resources/song2.ogg");
    bl::Playlist list3;
    list3.addSong("resources/song3.ogg");

    // playlists may be created manually
    bl::Playlist playlist;
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

    // We can save playlists to files
    std::cout << "Saving playlist\n";
    {
        bl::bf::BinaryFile output("mylist.plst", bl::bf::BinaryFile::Write);
        playlist.serialize(output);
    }

    // And load them
    bl::Playlist mylist("mylist.plst");
    std::cout << "Loaded playlist\n";

    // AudioSystem has a background thread which calls update() for us
    bl::AudioSystem system;
    system.pushPlaylist(list1);
    std::cout << "Playing playlist through AudioSystem\n";

    // AudioSystem also manages running sounds
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Playing thunder\n";
    system.playSound(bl::engine::Resources::sounds().load("resources/thunder.wav").data);

    // Sounds may be looped
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "Looping eagle\n";
    auto handle =
        system.playSound(bl::engine::Resources::sounds().load("resources/gameSave.wav").data, true);

    // Everything may be paused
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "Pausing\n";
    system.pause();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Unpausing\n";
    system.resume();

    // Sounds may be stopped using their handles
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "Stopping eagle\n";
    system.stopSound(handle);

    // AudioSystem maintains a stack of playlists
    system.pushPlaylist(list2);
    std::cout << "Pushing new playlist\n";
    std::this_thread::sleep_for(std::chrono::seconds(7));
    std::cout << "Popping it\n";
    system.popPlaylist();

    // Playlists may also be replaced
    std::this_thread::sleep_for(std::chrono::seconds(7));
    std::cout << "Replacing current playlist\n";
    system.replacePlaylist(list3); // replace current playing with mylist
    std::this_thread::sleep_for(std::chrono::seconds(7));

    // AudioSystem will fade out and clean up
    return 0;
}
