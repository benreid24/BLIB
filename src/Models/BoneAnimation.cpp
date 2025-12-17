#include <BLIB/Models/BoneAnimation.hpp>

namespace bl
{
namespace mdl
{
BoneAnimation::BoneAnimation()
: boneName()
, preBehavior(Default)
, postBehavior(Default) {}

void BoneAnimation::populate(const aiNodeAnim& src) {
    boneName = std::string(src.mNodeName.C_Str());
    positionKeys.resize(src.mNumPositionKeys);
    for (unsigned int i = 0; i < src.mNumPositionKeys; ++i) {
        positionKeys[i].populate(src.mPositionKeys[i]);
    }
    rotationKeys.resize(src.mNumRotationKeys);
    for (unsigned int i = 0; i < src.mNumRotationKeys; ++i) {
        rotationKeys[i].populate(src.mRotationKeys[i]);
    }
    scaleKeys.resize(src.mNumScalingKeys);
    for (unsigned int i = 0; i < src.mNumScalingKeys; ++i) {
        scaleKeys[i].populate(src.mScalingKeys[i]);
    }
    preBehavior  = static_cast<Behavior>(src.mPreState);
    postBehavior = static_cast<Behavior>(src.mPostState);
}

} // namespace mdl
} // namespace bl
