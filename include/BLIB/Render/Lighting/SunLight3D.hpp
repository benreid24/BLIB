#ifndef BLIB_RENDER_LIGHTNG_SUNLIGHT3D_HPP
#define BLIB_RENDER_LIGHTNG_SUNLIGHT3D_HPP

#include <BLIB/Render/Lighting/Color3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Basic struct representing a global directional light in a scene
 *
 * @ingroup Renderer
 */
struct alignas(16) SunLight3D {
    glm::vec3 dir;
    Color3D color;
    glm::mat4 viewProjectionMatrix;

    /**
     * @brief Creates the light with sane defaults
     */
    SunLight3D()
    : dir(glm::normalize(glm::vec3(-0.75f, -1.f, 0.25f)))
    , color() {
        color.ambient = glm::vec3(1.f);
    }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
