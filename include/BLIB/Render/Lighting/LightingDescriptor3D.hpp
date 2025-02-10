#ifndef BLIB_RENDER_LIGHTING_LIGHTINGDESCRIPTOR3D_HPP
#define BLIB_RENDER_LIGHTING_LIGHTINGDESCRIPTOR3D_HPP

#include <BLIB/Render/Lighting/SunLight3D.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Basic struct containing lighting info for 3d scenes (excluding point & spot lights)
 *
 * @ingroup Renderer
 */
struct alignas(16) LightingDescriptor3D {
    SunLight3D sun;
    alignas(16) glm::vec3 globalAmbient;
    std::uint32_t nPointLights;
    std::uint32_t nSpotLights;

    /**
     * @brief Initializes with sane defaults
     */
    LightingDescriptor3D()
    : sun()
    , globalAmbient(0.f)
    , nPointLights(0)
    , nSpotLights(0) {}
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
