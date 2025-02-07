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
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
