#ifndef BLIB_RENDER_COMPONENTS_MATERIAL_HPP
#define BLIB_RENDER_COMPONENTS_MATERIAL_HPP

#include <BLIB/Render/Uniforms/MaterialUniform.hpp>
#include <cstdint>

namespace bl
{
namespace render
{
/**
 * @brief Basic structure representing a material. A material is a combination of a texture with
 *        lighting properties. Materials are managed by the MaterialPool
 *
 * @ingroup Renderer
 */
struct Material {
    std::uint32_t textureId;
    MaterialUniform parameters;
};

} // namespace render
} // namespace bl

#endif
