#include <BLIB/Assets/Builtin/MusicPayload.hpp>
#include <BLIB/Assets/Drivers/MusicDriver.hpp>
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
TEST_F(RepositoryTest, MusicDriver) {
    constexpr unsigned int SampleRate   = 44100;
    constexpr unsigned int SampleCount  = 44100; // 1 second of mono audio
    constexpr unsigned int ChannelCount = 1;

    // Generate known samples (simple sawtooth wave)
    std::vector<std::int16_t> sourceSamples(SampleCount);
    for (unsigned int i = 0; i < SampleCount; ++i) {
        sourceSamples[i] = static_cast<std::int16_t>((i % 256) * 256 - 32768);
    }

    util::UUID uuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        asi::MusicDriver::CreateParams params;
        params.samples      = sourceSamples.data();
        params.sampleCount  = SampleCount;
        params.channelCount = ChannelCount;
        params.sampleRate   = SampleRate;
        params.channelMap   = {sf::SoundChannel::Mono};

        auto asset = repo.createAsset<asi::MusicPayload>("TestMusic", params);
        ASSERT_TRUE(asset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        uuid = asset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<asi::MusicPayload>(uuid);
    ASSERT_TRUE(asset.isValid());

    const sf::Music& loaded = asset->get();
    EXPECT_EQ(loaded.getSampleRate(), SampleRate);
    EXPECT_EQ(loaded.getChannelCount(), ChannelCount);

    // Duration should be approximately 1 second for SampleCount samples at SampleRate
    const float durationSeconds = loaded.getDuration().asSeconds();
    EXPECT_NEAR(durationSeconds, 1.0f, 0.05f);
}

} // namespace unittest
} // namespace as
} // namespace bl
