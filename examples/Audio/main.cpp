#include <BLIB/Audio.hpp>
#include <BLIB/Engine.hpp>

#include <chrono>
#include <iostream>
#include <thread>

using bl::audio::AudioSystem;

enum struct Step {
    PlayingBiglist,
    PlayingList1,
    PlayingThunder,
    PlayingLoopSound,
    Paused,
    Resumed,
    StoppedLoop,
    PlayingList2,
    PlayingList3,
};

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual ~DemoState() = default;

private:
    bl::util::UUID list1Id;
    bl::util::UUID list2Id;
    bl::util::UUID list3Id;
    bl::util::UUID biglistId;
    AudioSystem::Handle loopedSound;

    Step step;
    float timeRemaining = 0.f;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        auto song1 =
            engine.assets().getAssetFromSourcePath<bl::asi::MusicPayload>("resources/song1.ogg");
        auto song2 =
            engine.assets().getAssetFromSourcePath<bl::asi::MusicPayload>("resources/song2.ogg");
        auto song3 =
            engine.assets().getAssetFromSourcePath<bl::asi::MusicPayload>("resources/song3.ogg");

        auto list1 = engine.assets().createAsset<bl::asi::PlaylistPayload>("list1");
        list1->addSong(song1.getUUID());
        list1Id = list1.getUUID();

        auto list2 = engine.assets().createAsset<bl::asi::PlaylistPayload>("list2");
        list2->addSong(song2.getUUID());
        list2Id = list2.getUUID();

        auto list3 = engine.assets().createAsset<bl::asi::PlaylistPayload>("list3");
        list3->addSong(song3.getUUID());
        list3Id = list3.getUUID();

        auto biglist = engine.assets().createAsset<bl::asi::PlaylistPayload>("biglist");
        biglist->addSong(song1.getUUID());
        biglist->addSong(song2.getUUID());
        biglist->addSong(song3.getUUID());
        biglist->setShuffle(true);
        biglistId = biglist.getUUID();

        BL_LOG_INFO << "Playing playlist";
        AudioSystem::Handle biglistHandle = AudioSystem::getOrLoadPlaylist(biglist);
        AudioSystem::pushPlaylist(biglistHandle);
        step          = Step::PlayingBiglist;
        timeRemaining = 5.f;
    }

    virtual void deactivate(bl::engine::Engine&) override {}

    virtual void update(bl::engine::Engine& engine, float, float realDt) override {
        timeRemaining -= realDt;
        if (timeRemaining <= 0.f) {
            switch (step) {
            case Step::PlayingBiglist: {
                auto asset = engine.assets().getTypedAsset<bl::asi::PlaylistPayload>(list1Id);
                AudioSystem::Handle list1Handle = AudioSystem::getOrLoadPlaylist(asset);
                AudioSystem::pushPlaylist(list1Handle);
                step          = Step::PlayingList1;
                timeRemaining = 5.f;
                break;
            }
            case Step::PlayingList1: {
                BL_LOG_INFO << "Playing sound";
                auto sound = engine.assets().getAssetFromSourcePath<bl::asi::SoundPayload>(
                    "resources/thunder.wav");
                AudioSystem::Handle thunder = AudioSystem::getOrLoadSound(sound);
                AudioSystem::playSound(thunder);
                step          = Step::PlayingThunder;
                timeRemaining = 5.f;
                break;
            }
            case Step::PlayingThunder: {
                BL_LOG_INFO << "Playing looped sound";
                auto sound = engine.assets().getAssetFromSourcePath<bl::asi::SoundPayload>(
                    "resources/gameSave.wav");
                loopedSound = AudioSystem::getOrLoadSound(sound);
                AudioSystem::playSound(loopedSound, -1.f, true);
                step          = Step::PlayingLoopSound;
                timeRemaining = 4.f;
                break;
            }
            case Step::PlayingLoopSound: {
                BL_LOG_INFO << "Pausing audio";
                AudioSystem::pause();
                step          = Step::Paused;
                timeRemaining = 3.f;
                break;
            }
            case Step::Paused: {
                BL_LOG_INFO << "Resuming audio";
                AudioSystem::resume();
                step          = Step::Resumed;
                timeRemaining = 4.f;
                break;
            }
            case Step::Resumed: {
                BL_LOG_INFO << "Stopping looped sound";
                AudioSystem::stopSound(loopedSound);
                step          = Step::StoppedLoop;
                timeRemaining = 4.f;
                break;
            }
            case Step::StoppedLoop: {
                BL_LOG_INFO << "Playing another playlist with push";
                auto asset = engine.assets().getTypedAsset<bl::asi::PlaylistPayload>(list2Id);
                AudioSystem::Handle list2Handle = AudioSystem::getOrLoadPlaylist(asset);
                AudioSystem::pushPlaylist(list2Handle);
                step          = Step::PlayingList2;
                timeRemaining = 7.f;
                break;
            }
            case Step::PlayingList2: {
                BL_LOG_INFO << "Replacing current playlist with another";
                auto asset = engine.assets().getTypedAsset<bl::asi::PlaylistPayload>(list3Id);
                AudioSystem::Handle list3Handle = AudioSystem::getOrLoadPlaylist(asset);
                AudioSystem::replacePlaylist(list3Handle);
                step          = Step::PlayingList3;
                timeRemaining = 7.f;
                break;
            }
            case Step::PlayingList3: {
                BL_LOG_INFO << "Demo complete";
                engine.popState();
                break; // done
            }
            }
        }
    }
};

int main() {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Debug);
    const bl::engine::Settings engineSettings =
        bl::engine::Settings().withRenderer(bl::rc::CreationSettings().withWindowSettings(
            bl::rc::WindowSettings()
                .withVideoMode(sf::VideoMode({1920, 1080}, 32))
                .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
                .withTitle("Audio Demo")
                .withLetterBoxOnResize(true)));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
