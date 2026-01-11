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
struct alignas(32) MaterialDescriptor {
    std::uint32_t diffuseTextureId;
    std::uint32_t normalTextureId;
    std::uint32_t specularTextureId;
    std::uint32_t parallaxTextureId;
    float shininess;
    float heightScale;
    float padding0;
    float padding1;
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
