#ifndef BLIB_RENDER_LIGHTING_SPOTLIGHT3D_HPP
#define BLIB_RENDER_LIGHTING_SPOTLIGHT3D_HPP

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
 * @brief Basic struct representing a spotlight in a 3d scene
 *
 * @ingroup Renderer
 */
struct SpotLight3D {
    alignas(16) glm::vec3 pos;
    float cutoff;
    alignas(16) glm::vec3 dir;
    float outerCutoff;
    Attenuation attenuation;
    Color3D color;

    /**
     * @brief Creates the light with sane defaults
     */
    SpotLight3D()
    : pos()
    , dir(0.f, -1.f, 0.f)
    , cutoff(1.f)
    , outerCutoff(20.f)
    , attenuation()
    , color() {}

    /**
     * @brief Helper method to point the light at a specific position
     *
     * @param position The position to point at
     */
    void pointAt(const glm::vec3& position) { dir = glm::normalize(position - pos); }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
