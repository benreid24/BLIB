#ifndef BLIB_RENDER_LIGHTING_ATTENUATION_HPP
#define BLIB_RENDER_LIGHTING_ATTENUATION_HPP

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
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
