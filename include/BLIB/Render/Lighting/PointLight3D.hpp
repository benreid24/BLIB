#ifndef BLIB_RENDER_LIGHTING_POINTLIGHT3D_HPP
#define BLIB_RENDER_LIGHTING_POINTLIGHT3D_HPP

#include <BLIB/Render/Lighting/Attenuation.hpp>
#include <BLIB/Render/Lighting/Color3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Basic struct representing a 3d point light
 *
 * @ingroup Renderer
 */
struct alignas(16) PointLight3D {
    alignas(16) glm::vec3 pos;
    Attenuation attenuation;
    Color3D color;

    /**
     * @brief Initializes to sane defaults
     */
    PointLight3D() = default;

    /**
     * @brief Returns the radius of the light volume for this light
     *
     * @param threshold The threshold to consider the bounds of the light volume
     * @return The radius of the light volume
     */
    float computeFalloffRadius(float threshold = 5.f / 256.f) const {
        return attenuation.computeFalloffRadius(threshold, color.getMaxLightLevel());
    }

    /**
     * @brief Copies the light data from another light to this one which will be used by shaders
     *
     * @param other The light to copy from
     */
    void copyAsUniform(const PointLight3D& other) {
        pos         = other.pos;
        attenuation = other.attenuation;
        color       = other.color;
    }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
