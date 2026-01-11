#include <BLIB/Models/BoneAnimation.hpp>

#include <BLIB/Models/NodeSet.hpp>
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>

namespace bl
{
namespace mdl
{
namespace
{
glm::vec3 interpolate(BoneAnimation::Behavior preBehavior, BoneAnimation::Behavior postBehavior,
                      const glm::vec3& bindPose, const std::vector<KeyframeVector>& keys,
                      double time) {
    if (keys.empty()) { return bindPose; }
    if (keys.size() == 1) { return keys.front().value; }

    if (time < keys.front().time) {
        switch (preBehavior) {
        case BoneAnimation::Constant:
            return keys.front().value;
        case BoneAnimation::Linear: {
            // TODO - unsure if this is correct
            const auto& first  = keys.front();
            const auto& second = keys[1];
            const double dt    = second.time - first.time;
            if (std::abs(dt) < 1e-8) { return first.value; }
            const float t = static_cast<float>((time - first.time) / dt);
            return glm::mix(first.value, second.value, t);
        }
        case BoneAnimation::Repeat: {
            const double duration = keys.back().time - keys.front().time;
            if (std::abs(duration) < 1e-8) { return keys.front().value; }
            time = std::fmod(time - keys.front().time, duration);
            if (time < 0.0) { time += duration; }
            time += keys.front().time;
        } break;
        case BoneAnimation::Default:
        default:
            return bindPose;
        }
    }
    else if (time > keys.back().time) {
        switch (postBehavior) {
        case BoneAnimation::Constant:
            return keys.back().value;
        case BoneAnimation::Linear: {
            const auto& last       = keys.back();
            const auto& secondLast = keys[keys.size() - 2];
            const double dt        = last.time - secondLast.time;
            if (std::abs(dt) < 1e-8) { return last.value; }
            const float t = static_cast<float>((time - secondLast.time) / dt);
            return glm::mix(secondLast.value, last.value, t);
        }
        case BoneAnimation::Repeat: {
            const double duration = keys.back().time - keys.front().time;
            if (std::abs(duration) < 1e-8) { return keys.front().value; }
            time = std::fmod(time - keys.front().time, duration);
            if (time < 0.0) { time += duration; }
            time += keys.front().time;
        } break;
        default:
            return bindPose;
        }
    }

    const auto comp = [](const KeyframeVector& kf, double t) { return kf.time < t; };
    const auto next = std::lower_bound(keys.begin(), keys.end(), time, comp);
    if (next == keys.end()) { return keys.back().value; }
    if (next == keys.begin()) { return next->value; }

    const auto current = next - 1;
    const double dt    = next->time - current->time;
    if (std::abs(dt) < 1e-8) { return current->value; }
    const float t = (time - current->time) / dt;

    switch (current->interpolation) {
    case KeyframeVector::Step:
        return current->value;
    case KeyframeVector::SphericalLinear:
    case KeyframeVector::CubicSpline:
    case KeyframeVector::Linear:
        return glm::mix(current->value, next->value, t);
    default:
        return bindPose;
    }
}

glm::quat slerp(const glm::quat& a, const glm::quat& b, float t) {
    glm::quat q1 = b;
    if (glm::dot(a, b) < 0.f) { q1 = -b; }
    return glm::slerp(a, q1, t);
}

} // namespace

BoneAnimation::BoneAnimation()
: boneName()
, preBehavior(Default)
, postBehavior(Default)
, bindPosePosition(0.f)
, bindPoseScale(1.f)
, bindPoseRotation(1.f, 0.f, 0.f, 0.f) {}

void BoneAnimation::populate(const aiNodeAnim& src, const NodeSet& nodes) {
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

    const Node* boneNode = nodes.getNode(boneName);
    if (boneNode) {
        const glm::mat4& localTransform = boneNode->getTransform();
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(
            localTransform, bindPoseScale, bindPoseRotation, bindPosePosition, skew, perspective);
    }
}

glm::quat BoneAnimation::interpolateRotation(double time) const {
    if (rotationKeys.empty()) { return bindPoseRotation; }
    if (rotationKeys.size() == 1) { return rotationKeys.front().value; }

    if (time < rotationKeys.front().time) {
        switch (preBehavior) {
        case BoneAnimation::Constant:
            return rotationKeys.front().value;
        case BoneAnimation::Linear: {
            // TODO - unsure if this is correct
            const auto& first  = rotationKeys.front();
            const auto& second = rotationKeys[1];
            const double dt    = second.time - first.time;
            if (std::abs(dt) < 1e-8) { return first.value; }
            const float t = static_cast<float>((time - first.time) / dt);
            return slerp(first.value, second.value, t);
        }
        case BoneAnimation::Repeat: {
            const double duration = rotationKeys.back().time - rotationKeys.front().time;
            if (std::abs(duration) < 1e-8) { return rotationKeys.front().value; }
            time = std::fmod(time - rotationKeys.front().time, duration);
            if (time < 0.0) { time += duration; }
            time += rotationKeys.front().time;
        } break;
        case BoneAnimation::Default:
        default:
            return bindPoseRotation;
        }
    }
    else if (time > rotationKeys.back().time) {
        switch (postBehavior) {
        case BoneAnimation::Constant:
            return rotationKeys.back().value;
        case BoneAnimation::Linear: {
            const auto& last       = rotationKeys.back();
            const auto& secondLast = rotationKeys[rotationKeys.size() - 2];
            const double dt        = last.time - secondLast.time;
            if (std::abs(dt) < 1e-8) { return last.value; }
            const float t = static_cast<float>((time - secondLast.time) / dt);
            return slerp(secondLast.value, last.value, t);
        }
        case BoneAnimation::Repeat: {
            const double duration = rotationKeys.back().time - rotationKeys.front().time;
            if (std::abs(duration) < 1e-8) { return rotationKeys.front().value; }
            time = std::fmod(time - rotationKeys.front().time, duration);
            if (time < 0.0) { time += duration; }
            time += rotationKeys.front().time;
        } break;
        default:
            return bindPoseRotation;
        }
    }

    const auto comp = [](const KeyframeQuaternion& kf, double t) { return kf.time < t; };
    const auto next = std::lower_bound(rotationKeys.begin(), rotationKeys.end(), time, comp);
    if (next == rotationKeys.end()) { return rotationKeys.back().value; }
    if (next == rotationKeys.begin()) { return next->value; }

    const auto current = next - 1;
    const double dt    = next->time - current->time;
    if (std::abs(dt) < 1e-8) { return current->value; }
    const float t = (time - current->time) / dt;

    switch (current->interpolation) {
    case KeyframeQuaternion::Step:
        return current->value;
    case KeyframeQuaternion::Linear:
    case KeyframeQuaternion::SphericalLinear:
    case KeyframeQuaternion::CubicSpline: // nah, they get spherical lerp
        return slerp(current->value, next->value, t);

    default:
        return bindPoseRotation;
    }
}

glm::vec3 BoneAnimation::interpolatePosition(double time) const {
    return interpolate(preBehavior, postBehavior, bindPosePosition, positionKeys, time);
}

glm::vec3 BoneAnimation::interpolateScale(double time) const {
    return interpolate(preBehavior, postBehavior, bindPoseScale, scaleKeys, time);
}

} // namespace mdl
} // namespace bl
