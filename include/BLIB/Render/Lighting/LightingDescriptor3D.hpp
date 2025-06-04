#ifndef BLIB_RENDER_LIGHTING_LIGHTINGDESCRIPTOR3D_HPP
#define BLIB_RENDER_LIGHTING_LIGHTINGDESCRIPTOR3D_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/PointLight3DShadow.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3DShadow.hpp>
#include <BLIB/Render/Lighting/SunLight3D.hpp>
#include <array>

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
    std::uint32_t nPointShadows;
    std::uint32_t nSpotShadows;
    std::array<SpotLight3D, Config::MaxSpotLights> spotlights;
    std::array<SpotLight3DShadow, Config::MaxSpotShadows> spotlightsWithShadows;
    std::array<PointLight3D, Config::MaxPointLights> pointLights;
    std::array<PointLight3DShadow, Config::MaxPointShadows> pointLightsWithShadows;

    /**
     * @brief Initializes with sane defaults
     */
    LightingDescriptor3D()
    : sun()
    , globalAmbient(0.f)
    , nPointLights(0)
    , nSpotLights(0)
    , nPointShadows(0)
    , nSpotShadows(0) {}
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
