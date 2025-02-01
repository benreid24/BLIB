#ifndef BLIB_RENDER_MATERIALS_MATERIALDESCRIPTOR_HPP
#define BLIB_RENDER_MATERIALS_MATERIALDESCRIPTOR_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace mat
{
/**
 * @brief Material data that gets sent to the GPU for use in shaders
 *
 * @ingroup Renderer
 */
struct MaterialDescriptor {
    std::uint32_t diffuseTextureId;
    std::uint32_t normalTextureId;
    std::uint32_t uvTextureId;

    std::uint32_t padding;
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
