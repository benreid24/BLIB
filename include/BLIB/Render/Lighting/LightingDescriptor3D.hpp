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

    /**
     * @brief Returns the byte offset into the uniform to the camera matrices for the given light
     *
     * @param i The index of the spot light
     * @return The byte offset for the light's camera matrices
     */
    std::uint32_t getMatrixOffsetForSpotLight(std::uint32_t i) const {
        constexpr std::uint32_t BaseOffset = offsetof(LightingDescriptor3D, spotlightsWithShadows);
        return BaseOffset + i * sizeof(SpotLight3DShadow) + SpotLight3DShadow::getMatrixOffset();
    }

    /**
     * @brief Returns the byte offset into the uniform to the camera matrices for the given light
     *
     * @param i The index of the point light
     * @return The byte offset for the light's camera matrices
     */
    std::uint32_t getMatrixOffsetForPointLight(std::uint32_t i) const {
        constexpr std::uint32_t BaseOffset = offsetof(LightingDescriptor3D, pointLightsWithShadows);
        return BaseOffset + i * sizeof(PointLight3DShadow) + PointLight3DShadow::getMatrixOffset();
    }

    static constexpr std::uint32_t getSunlightCameraMatrixOffset() {
        return offsetof(LightingDescriptor3D, sun) + SunLight3D::getMatrixOffset();
    }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
