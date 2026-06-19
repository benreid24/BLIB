#include <BLIB/Assets/Builtin/Animation2DPayload.hpp>
#include <BLIB/Assets/Builtin/Animation2DSetPayload.hpp>
#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Drivers/Animation2DDriver.hpp>
#include <BLIB/Assets/Drivers/Animation2DSetDriver.hpp>
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
TEST_F(RepositoryTest, Animation2DSetDriver) {
    // Base animation: 2 frames, 1 shard each, looping
    asi::Animation2DPayload::Frame baseFrame0;
    baseFrame0.length        = 0.1f;
    auto& bs0                = baseFrame0.shards.emplace_back();
    bs0.source               = sf::IntRect({0, 0}, {16, 16});
    bs0.offset               = {0.0f, 0.0f};
    bs0.scale                = {1.0f, 1.0f};
    bs0.rotation             = 0.0f;
    bs0.alpha                = 255;

    asi::Animation2DPayload::Frame baseFrame1;
    baseFrame1.length        = 0.15f;
    auto& bs1                = baseFrame1.shards.emplace_back();
    bs1.source               = sf::IntRect({16, 0}, {16, 16});
    bs1.offset               = {0.0f, 0.0f};
    bs1.scale                = {1.0f, 1.0f};
    bs1.rotation             = 0.0f;
    bs1.alpha                = 255;

    // State animation: 2 frames, 1 shard each (same spritesheet, compatible shard count)
    asi::Animation2DPayload::Frame stateFrame0;
    stateFrame0.length       = 0.2f;
    auto& ss0                = stateFrame0.shards.emplace_back();
    ss0.source               = sf::IntRect({0, 16}, {16, 16});
    ss0.offset               = {0.0f, 0.0f};
    ss0.scale                = {1.0f, 1.0f};
    ss0.rotation             = 0.0f;
    ss0.alpha                = 200;

    asi::Animation2DPayload::Frame stateFrame1;
    stateFrame1.length       = 0.25f;
    auto& ss1                = stateFrame1.shards.emplace_back();
    ss1.source               = sf::IntRect({16, 16}, {16, 16});
    ss1.offset               = {0.0f, 0.0f};
    ss1.scale                = {1.0f, 1.0f};
    ss1.rotation             = 0.0f;
    ss1.alpha                = 150;

    sf::Image sourceImage;
    sourceImage.resize({32, 32}, sf::Color::White);

    util::UUID setUuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        asi::ImageDriver::CreateParams imageParams;
        imageParams.sourceImage = &sourceImage;
        auto imageAsset         = repo.createAsset<asi::ImagePayload>("TestSetSheet", imageParams);
        ASSERT_TRUE(imageAsset.isValid());

        asi::Animation2DDriver::CreateParams baseParams;
        baseParams.spritesheet  = imageAsset;
        baseParams.frames       = {baseFrame0, baseFrame1};
        baseParams.loop         = true;
        baseParams.centerShards = false;
        auto baseAnim = repo.createAsset<asi::Animation2DPayload>("TestBaseAnim", baseParams);
        ASSERT_TRUE(baseAnim.isValid());

        asi::Animation2DDriver::CreateParams stateParams;
        stateParams.spritesheet  = imageAsset;
        stateParams.frames       = {stateFrame0, stateFrame1};
        stateParams.loop         = true;
        stateParams.centerShards = false;
        auto stateAnim = repo.createAsset<asi::Animation2DPayload>("TestStateAnim", stateParams);
        ASSERT_TRUE(stateAnim.isValid());

        asi::Animation2DSetPayload::CreateData setData;
        setData.baseAnimation         = baseAnim;
        setData.additionalAnimations  = {stateAnim};
        setData.enforceSlideshow      = false;
        auto setAsset = repo.createAsset<asi::Animation2DSetPayload>("TestAnimSet", setData);
        ASSERT_TRUE(setAsset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        setUuid = setAsset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto setAsset = repo.getTypedAsset<asi::Animation2DSetPayload>(setUuid);
    ASSERT_TRUE(setAsset.isValid());

    const asi::Animation2DSetPayload& set = *setAsset;

    // 2 states × 2 frames each = 4 total frames
    EXPECT_EQ(set.frameCount(), 4u);
    EXPECT_TRUE(set.isLooping());
    EXPECT_FALSE(set.shardsAreCentered());

    // State start indices
    EXPECT_EQ(set.getFrameForState(0), 0u);
    EXPECT_EQ(set.getFrameForState(1), 2u);

    // State lookup from frame
    EXPECT_EQ(set.getStateFromFrame(0), 0u);
    EXPECT_EQ(set.getStateFromFrame(1), 0u);
    EXPECT_EQ(set.getStateFromFrame(2), 1u);
    EXPECT_EQ(set.getStateFromFrame(3), 1u);

    // Frame lengths round-trip
    EXPECT_NEAR(set.getFrameLength(0), baseFrame0.length, 0.001f);
    EXPECT_NEAR(set.getFrameLength(1), baseFrame1.length, 0.001f);
    EXPECT_NEAR(set.getFrameLength(2), stateFrame0.length, 0.001f);
    EXPECT_NEAR(set.getFrameLength(3), stateFrame1.length, 0.001f);

    // Next-frame wrapping stays within each state
    EXPECT_EQ(set.getNextFrame(0), 1u); // state 0: frame 0 -> frame 1
    EXPECT_EQ(set.getNextFrame(1), 0u); // state 0: frame 1 wraps -> frame 0
    EXPECT_EQ(set.getNextFrame(2), 3u); // state 1: frame 0 -> frame 1
    EXPECT_EQ(set.getNextFrame(3), 2u); // state 1: frame 1 wraps -> frame 2 (offset)

    // Spot-check shard data on frame 2 (state 1, frame 0)
    const auto& f2 = set.getFrame(2);
    ASSERT_EQ(f2.shards.size(), 1u);
    EXPECT_EQ(f2.shards[0].source, ss0.source);
    EXPECT_EQ(f2.shards[0].alpha, ss0.alpha);
}

} // namespace unittest
} // namespace as
} // namespace bl
