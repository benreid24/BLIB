#ifndef BLIB_RENDER_UNIFORMS_MATERIALUNIFORM_HPP
#define BLIB_RENDER_UNIFORMS_MATERIALUNIFORM_HPP

#include <glm/glm.hpp>

namespace bl
{
namespace render
{
/**
 * @brief POD containing the data that a material sends to the shaders
 * 
 * @ingroup Renderer
 */
struct MaterialUniform {
    /// @brief Creates the uniform with sane defaults
    MaterialUniform();

    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
    alignas(4) float shininess;
};

} // namespace render
} // namespace bl

#endif
