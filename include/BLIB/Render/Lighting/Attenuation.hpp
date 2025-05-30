#ifndef BLIB_RENDER_LIGHTING_ATTENUATION_HPP
#define BLIB_RENDER_LIGHTING_ATTENUATION_HPP

#include <cmath>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Basic struct containing attenuation parameters for 3d lights
 *
 * @ingroup Renderer
 */
struct alignas(16) Attenuation {
    float constant;
    float linear;
    float quadratic;

    /**
     * @brief Initializes the factors to sane defaults
     */
    Attenuation()
    : constant(1.f)
    , linear(0.22f)
    , quadratic(0.2f) {}

    /**
     * @brief Computes the effective radius of the light with this attenuation
     *
     * @param threshold The light level threshold to consider the bounds of the light volume
     * @param lightMax The maximum light level of the light
     * @return The radius of the light volume at the given threshold
     */
    float computeFalloffRadius(float threshold = 256.f / 5.f, float lightMax = 1.f) const {
        return (-linear +
                std::sqrtf(linear * linear - 4 * quadratic * (constant - threshold * lightMax))) /
               (2 * quadratic);
    }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
