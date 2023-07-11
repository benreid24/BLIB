#ifndef BLIB_RENDER_RENDERER_MATERIAL_HPP
#define BLIB_RENDER_RENDERER_MATERIAL_HPP

#include <cstdint>

namespace bl
{
namespace gfx
{
namespace res
{
/**
 * @brief Basic structure representing a material. A material is a combination of a texture with
 *        lighting properties. Materials are managed by the MaterialPool
 *
 * @ingroup Renderer
 */
struct Material {
    std::uint32_t textureId;
    // MaterialUniform parameters;
};

} // namespace res
} // namespace gfx
} // namespace bl

#endif
