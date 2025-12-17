#ifndef BLIB_MODELS_KEYFRAMEQUATERNION_HPP
#define BLIB_MODELS_KEYFRAMEQUATERNION_HPP

#include <assimp/anim.h>
#include <cstdint>
#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace mdl
{
/**
 * @brief A keyframe for a quaternion value in an animation
 *
 * @ingroup Models
 */
struct KeyframeQuaternion {
    /**
     * @brief The interpolation type for the keyframe
     */
    enum Interpolation : std::uint8_t {
        Step            = aiAnimInterpolation_Step,
        Linear          = aiAnimInterpolation_Linear,
        SphericalLinear = aiAnimInterpolation_Spherical_Linear,
        CubicSpline     = aiAnimInterpolation_Cubic_Spline
    };

    double time;
    glm::quat value;
    Interpolation interpolation;

    /**
     * @brief Creates an empty keyframe
     */
    KeyframeQuaternion();

    /**
     * @brief Populates the keyframe from the given assimp quaternion key
     *
     * @param src The Assimp keyframe
     */
    void populate(const aiQuatKey& src);
};

} // namespace mdl
} // namespace bl

#endif
