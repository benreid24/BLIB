#ifndef BLIB_MODELS_BONEANIMATION_HPP
#define BLIB_MODELS_BONEANIMATION_HPP

#include <BLIB/Models/KeyframeQuaternion.hpp>
#include <BLIB/Models/KeyframeVector.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace bl
{
namespace mdl
{
class NodeSet;

/**
 * @brief Describes the animation of a single bone in a skeletal animation
 *
 * @ingroup Models
 */
class BoneAnimation {
public:
    /**
     * @brief The behavior of the animation before and after its keyframes
     */
    enum Behavior : std::uint8_t {
        /// The value from the default node transformation is taken
        Default = aiAnimBehaviour_DEFAULT,

        /// The nearest key value is used without interpolation
        Constant = aiAnimBehaviour_CONSTANT,

        /// The value of the nearest two keys is linearly extrapolated for the current time value
        Linear = aiAnimBehaviour_LINEAR,

        /// The animation is repeated
        Repeat = aiAnimBehaviour_REPEAT
    };

    /**
     * @brief Creates an empty bone animation
     */
    BoneAnimation();

    /**
     * @brief Populates from Assimp data
     *
     * @param src The source data
     * @param nodes The set of nodes in the model
     */
    void populate(const aiNodeAnim& src, const NodeSet& nodes);

    /**
     * @brief Returns the interpolated rotation at the given time
     *
     * @param time The time to sample at
     * @return The interpolated rotation
     */
    glm::quat interpolateRotation(double time) const;

    /**
     * @brief Returns the interpolated position at the given time
     *
     * @param time The time to sample at
     * @return The interpolated position
     */
    glm::vec3 interpolatePosition(double time) const;

    /**
     * @brief Returns the interpolated scale at the given time
     *
     * @param time The time to sample at
     * @return The interpolated scale
     */
    glm::vec3 interpolateScale(double time) const;

    /**
     * @brief Returns the name of the animated bone
     */
    const std::string& getBoneName() const { return boneName; }

    /**
     * @brief Returns the position keyframes of the animation
     */
    const std::vector<KeyframeVector>& getPositionKeys() const { return positionKeys; }

    /**
     * @brief Returns the rotation keyframes of the animation
     */
    const std::vector<KeyframeQuaternion>& getRotationKeys() const { return rotationKeys; }

    /**
     * @brief Returns the scale keyframes of the animation
     */
    const std::vector<KeyframeVector>& getScaleKeys() const { return scaleKeys; }

    /**
     * @brief Returns the behavior of the animation before the first keyframe
     */
    Behavior getPreStartBehavior() const { return preBehavior; }

    /**
     * @brief Returns the behavior of the animation after the last keyframe
     */
    Behavior getPostDoneBehavior() const { return postBehavior; }

private:
    std::string boneName;
    std::vector<KeyframeVector> positionKeys;
    std::vector<KeyframeQuaternion> rotationKeys;
    std::vector<KeyframeVector> scaleKeys;
    Behavior preBehavior;
    Behavior postBehavior;
    glm::vec3 bindPosePosition;
    glm::vec3 bindPoseScale;
    glm::quat bindPoseRotation;
};

} // namespace mdl
} // namespace bl

#endif
