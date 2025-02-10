#ifndef BLIB_RENDER_LIGHTING_COLOR3D_HPP
#define BLIB_RENDER_LIGHTING_COLOR3D_HPP

#include <BLIB/Render/Color.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Basic struct containing the color of a 3d light
 *
 * @ingroup Renderer
 */
struct alignas(16) Color3D {
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;

    /**
     * @brief Initializes all colors to black (no light)
     */
    Color3D() = default;

    /**
     * @brief Convenience method to assign all 3 colors to a single color
     *
     * @param color The color to set to
     * @return A reference to this object
     */
    Color3D& operator=(const Color& color) {
        ambient  = color;
        diffuse  = color;
        specular = color;
        return *this;
    }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
