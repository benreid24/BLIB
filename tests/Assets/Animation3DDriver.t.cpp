#include <BLIB/Assets/Builtin/Animation3DPayload.hpp>
#include <BLIB/Assets/Drivers/Animation3DDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Models/Animation.hpp>
#include <BLIB/Models/BoneAnimation.hpp>
#include <BLIB/Models/KeyframeQuaternion.hpp>
#include <BLIB/Models/KeyframeVector.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, Animation3DDriver) {
    // Build a known animation: 1 bone, 2 position keyframes, 1 rotation keyframe, 1 scale keyframe
    mdl::BoneAnimation boneAnim;
    boneAnim.init("TestBone",
                  mdl::BoneAnimation::Repeat,
                  mdl::BoneAnimation::Constant,
                  glm::vec3(1.0f, 2.0f, 3.0f),
                  glm::vec3(1.0f, 1.0f, 1.0f),
                  glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    mdl::KeyframeVector pos0;
    pos0.time          = 0.0;
    pos0.value         = {0.0f, 0.0f, 0.0f};
    pos0.interpolation = mdl::KeyframeVector::Linear;
    boneAnim.addPositionKey(pos0);

    mdl::KeyframeVector pos1;
    pos1.time          = 1.0;
    pos1.value         = {5.0f, 3.0f, 1.0f};
    pos1.interpolation = mdl::KeyframeVector::Linear;
    boneAnim.addPositionKey(pos1);

    mdl::KeyframeQuaternion rot0;
    rot0.time          = 0.0;
    rot0.value         = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    rot0.interpolation = mdl::KeyframeQuaternion::SphericalLinear;
    boneAnim.addRotationKey(rot0);

    mdl::KeyframeVector scale0;
    scale0.time          = 0.0;
    scale0.value         = {2.0f, 2.0f, 2.0f};
    scale0.interpolation = mdl::KeyframeVector::Step;
    boneAnim.addScaleKey(scale0);

    mdl::Animation sourceAnim;
    sourceAnim.init("WalkCycle", 60.0, 30.0);
    sourceAnim.addBoneAnimation(std::move(boneAnim));

    util::UUID uuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);
        asi::Animation3DDriver::CreateParams params(sourceAnim);
        auto asset = repo.createAsset<asi::Animation3DPayload>("TestAnim3D", params);
        ASSERT_TRUE(asset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        uuid = asset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<asi::Animation3DPayload>(uuid);
    ASSERT_TRUE(asset.isValid());

    const mdl::Animation& loaded = asset->get();
    EXPECT_EQ(loaded.getName(), sourceAnim.getName());
    EXPECT_DOUBLE_EQ(loaded.getDurationInTicks(), sourceAnim.getDurationInTicks());
    EXPECT_DOUBLE_EQ(loaded.getTicksPerSecond(), sourceAnim.getTicksPerSecond());

    ASSERT_EQ(loaded.getBoneAnimations().size(), 1u);
    const mdl::BoneAnimation& loadedBone = loaded.getBoneAnimations()[0];
    EXPECT_EQ(loadedBone.getBoneName(), "TestBone");
    EXPECT_EQ(loadedBone.getPreStartBehavior(), mdl::BoneAnimation::Repeat);
    EXPECT_EQ(loadedBone.getPostDoneBehavior(), mdl::BoneAnimation::Constant);

    ASSERT_EQ(loadedBone.getPositionKeys().size(), 2u);
    EXPECT_DOUBLE_EQ(loadedBone.getPositionKeys()[0].time, pos0.time);
    EXPECT_EQ(loadedBone.getPositionKeys()[0].value, pos0.value);
    EXPECT_EQ(loadedBone.getPositionKeys()[0].interpolation, pos0.interpolation);
    EXPECT_DOUBLE_EQ(loadedBone.getPositionKeys()[1].time, pos1.time);
    EXPECT_EQ(loadedBone.getPositionKeys()[1].value, pos1.value);

    ASSERT_EQ(loadedBone.getRotationKeys().size(), 1u);
    EXPECT_DOUBLE_EQ(loadedBone.getRotationKeys()[0].time, rot0.time);
    EXPECT_EQ(loadedBone.getRotationKeys()[0].value, rot0.value);
    EXPECT_EQ(loadedBone.getRotationKeys()[0].interpolation, rot0.interpolation);

    ASSERT_EQ(loadedBone.getScaleKeys().size(), 1u);
    EXPECT_DOUBLE_EQ(loadedBone.getScaleKeys()[0].time, scale0.time);
    EXPECT_EQ(loadedBone.getScaleKeys()[0].value, scale0.value);
    EXPECT_EQ(loadedBone.getScaleKeys()[0].interpolation, scale0.interpolation);
}

} // namespace unittest
} // namespace as
} // namespace bl
