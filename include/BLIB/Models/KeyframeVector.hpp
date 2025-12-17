#ifndef BLIB_MODELS_KEYFRAMEVECTOR_HPP
#define BLIB_MODELS_KEYFRAMEVECTOR_HPP

#include <assimp/anim.h>
#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace mdl
{
/**
 * @brief A keyframe for a vector3 value in an animation
 *
 * @ingroup Models
 */
struct KeyframeVector {
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
    glm::vec3 value;
    Interpolation interpolation;

    /**
     * @brief Creates an empty keyframe
     */
    KeyframeVector();

    /**
     * @brief Populates the keyframe from the given assimp vector key
     *
     * @param src The Assimp keyframe
     */
    void populate(const aiVectorKey& src);
};

} // namespace mdl
} // namespace bl

#endif
