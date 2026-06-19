#include <BLIB/Assets/Builtin/MusicPayload.hpp>
#include <BLIB/Assets/Builtin/PlaylistPayload.hpp>
#include <BLIB/Assets/Drivers/MusicDriver.hpp>
#include <BLIB/Assets/Drivers/PlaylistDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

#include <cstdint>
#include <vector>

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, PlaylistDriver) {
    constexpr unsigned int SampleRate   = 44100;
    constexpr unsigned int SampleCount  = 44100; // 1 second of mono audio
    constexpr unsigned int ChannelCount = 1;

    std::vector<std::int16_t> samples(SampleCount);
    for (unsigned int i = 0; i < SampleCount; ++i) {
        samples[i] = static_cast<std::int16_t>((i % 256) * 256 - 32768);
    }

    util::UUID playlistUuid;
    std::array<util::UUID, 2> songUuids;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        for (int i = 0; i < 2; ++i) {
            asi::MusicDriver::CreateParams musicParams;
            musicParams.samples      = samples.data();
            musicParams.sampleCount  = SampleCount;
            musicParams.channelCount = ChannelCount;
            musicParams.sampleRate   = SampleRate;
            musicParams.channelMap   = {sf::SoundChannel::Mono};

            auto songAsset = repo.createAsset<asi::MusicPayload>(
                std::string("PlaylistSong") + std::to_string(i), musicParams);
            ASSERT_TRUE(songAsset.isValid());
            songUuids[i] = songAsset.getUUID();
        }

        // Create an empty playlist then mutate it via the returned ref
        auto playlistAsset = repo.createAsset<asi::PlaylistPayload>("TestPlaylist");
        ASSERT_TRUE(playlistAsset.isValid());
        ASSERT_TRUE(playlistAsset->addSong(songUuids[0]));
        ASSERT_TRUE(playlistAsset->addSong(songUuids[1]));
        playlistAsset->setShuffle(false);
        playlistAsset->setReshuffleOnLoop(false);

        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        playlistUuid = playlistAsset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto playlistAsset = repo.getTypedAsset<asi::PlaylistPayload>(playlistUuid);
    ASSERT_TRUE(playlistAsset.isValid());

    const asi::PlaylistPayload& playlist = *playlistAsset;
    EXPECT_FALSE(playlist.getShuffle());
    EXPECT_FALSE(playlist.getReshuffleOnLoop());
    ASSERT_EQ(playlist.getSongCount(), 2u);

    for (unsigned int i = 0; i < 2; ++i) {
        auto song = playlist.getSong(i);
        ASSERT_TRUE(song.isValid()) << "Song " << i << " is not valid";
        EXPECT_EQ(song->get().getSampleRate(), SampleRate) << "Song " << i << " sample rate mismatch";
        EXPECT_EQ(song->get().getChannelCount(), ChannelCount) << "Song " << i << " channel count mismatch";
    }
}

} // namespace unittest
} // namespace as
} // namespace bl
