#ifndef BLIB_MODELS_MATERIAL_HPP
#define BLIB_MODELS_MATERIAL_HPP

#include <BLIB/Models/Texture.hpp>
#include <string>

namespace bl
{
namespace mdl
{
/**
 * @brief Basic struct with material info for a mesh
 *
 * @ingroup Models
 */
struct Material {
    Texture diffuse;
    Texture normal;
    Texture specular;
    Texture parallax;
    float shininess;
};

} // namespace mdl
} // namespace bl

#endif
