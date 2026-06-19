#include <BLIB/Assets/Builtin/Animation2DPayload.hpp>
#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Drivers/Animation2DDriver.hpp>
#include <BLIB/Assets/Drivers/ImageDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <SFML/Graphics/Image.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, Animation2DDriver) {
    // Build a minimal 2-frame animation with 1 shard per frame
    asi::Animation2DPayload::Frame frame0;
    frame0.length              = 0.1f;
    auto& shard0               = frame0.shards.emplace_back();
    shard0.source              = sf::IntRect({0, 0}, {16, 16});
    shard0.offset              = {0.0f, 0.0f};
    shard0.scale               = {1.0f, 1.0f};
    shard0.rotation            = 0.0f;
    shard0.alpha               = 255;

    asi::Animation2DPayload::Frame frame1;
    frame1.length              = 0.2f;
    auto& shard1               = frame1.shards.emplace_back();
    shard1.source              = sf::IntRect({16, 0}, {16, 16});
    shard1.offset              = {4.0f, 2.0f};
    shard1.scale               = {2.0f, 2.0f};
    shard1.rotation            = 45.0f;
    shard1.alpha               = 128;

    sf::Image sourceImage;
    sourceImage.resize({32, 16}, sf::Color::White);

    util::UUID animUuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        asi::ImageDriver::CreateParams imageParams;
        imageParams.sourceImage = &sourceImage;
        auto imageAsset         = repo.createAsset<asi::ImagePayload>("TestAnimSheet", imageParams);
        ASSERT_TRUE(imageAsset.isValid());

        asi::Animation2DDriver::CreateParams animParams;
        animParams.spritesheet = imageAsset;
        animParams.frames      = {frame0, frame1};
        animParams.loop        = true;
        animParams.centerShards = false;

        auto animAsset = repo.createAsset<asi::Animation2DPayload>("TestAnim", animParams);
        ASSERT_TRUE(animAsset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        animUuid = animAsset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto animAsset = repo.getTypedAsset<asi::Animation2DPayload>(animUuid);
    ASSERT_TRUE(animAsset.isValid());

    const asi::Animation2DPayload& anim = *animAsset;
    EXPECT_TRUE(anim.isLooping());
    EXPECT_FALSE(anim.shardsAreCentered());
    EXPECT_NEAR(anim.getDuration(), frame0.length + frame1.length, 0.001f);

    ASSERT_EQ(anim.getFrames().size(), 2u);

    const auto& f0 = anim.getFrames()[0];
    EXPECT_NEAR(f0.length, frame0.length, 0.001f);
    ASSERT_EQ(f0.shards.size(), 1u);
    EXPECT_EQ(f0.shards[0].source, shard0.source);
    EXPECT_EQ(f0.shards[0].offset, shard0.offset);
    EXPECT_EQ(f0.shards[0].scale, shard0.scale);
    EXPECT_NEAR(f0.shards[0].rotation, shard0.rotation, 0.001f);
    EXPECT_EQ(f0.shards[0].alpha, shard0.alpha);

    const auto& f1 = anim.getFrames()[1];
    EXPECT_NEAR(f1.length, frame1.length, 0.001f);
    ASSERT_EQ(f1.shards.size(), 1u);
    EXPECT_EQ(f1.shards[0].source, shard1.source);
    EXPECT_EQ(f1.shards[0].offset, shard1.offset);
    EXPECT_EQ(f1.shards[0].scale, shard1.scale);
    EXPECT_NEAR(f1.shards[0].rotation, shard1.rotation, 0.001f);
    EXPECT_EQ(f1.shards[0].alpha, shard1.alpha);
}

} // namespace unittest
} // namespace as
} // namespace bl
