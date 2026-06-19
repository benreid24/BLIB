#include <BLIB/Assets/Builtin/SoundPayload.hpp>
#include <BLIB/Assets/Drivers/SoundDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
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
TEST_F(RepositoryTest, SoundDriver) {
    constexpr unsigned int SampleRate   = 44100;
    constexpr unsigned int SampleCount  = 4410; // 0.1 seconds of mono audio
    constexpr unsigned int ChannelCount = 1;

    // Generate known samples (simple sawtooth wave)
    std::vector<std::int16_t> sourceSamples(SampleCount);
    for (unsigned int i = 0; i < SampleCount; ++i) {
        sourceSamples[i] = static_cast<std::int16_t>((i % 256) * 256 - 32768);
    }

    sf::SoundBuffer sourceBuffer;
    ASSERT_TRUE(sourceBuffer.loadFromSamples(sourceSamples.data(),
                                             SampleCount,
                                             ChannelCount,
                                             SampleRate,
                                             {sf::SoundChannel::Mono}));

    util::UUID uuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);
        asi::SoundDriver::CreateParams params;
        params.sourceBuffer = &sourceBuffer;
        auto asset          = repo.createAsset<asi::SoundPayload>("TestSound", params);
        ASSERT_TRUE(asset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        uuid = asset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<asi::SoundPayload>(uuid);
    ASSERT_TRUE(asset.isValid());

    const sf::SoundBuffer& loaded = asset->get();
    EXPECT_EQ(loaded.getSampleRate(), SampleRate);
    EXPECT_EQ(loaded.getChannelCount(), ChannelCount);
    ASSERT_EQ(loaded.getSampleCount(), SampleCount);
    const std::int16_t* loadedSamples = loaded.getSamples();
    for (unsigned int i = 0; i < SampleCount; ++i) {
        EXPECT_EQ(loadedSamples[i], sourceSamples[i]) << "Sample mismatch at index " << i;
    }
}

} // namespace unittest
} // namespace as
} // namespace bl
