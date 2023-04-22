#ifndef BLIB_RENDER_RENDERER_DRAWPARAMETERS_HPP
#define BLIB_RENDER_RENDERER_DRAWPARAMETERS_HPP

#include <cstdint>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
/**
 * @brief Collection of the required parameters to issue draw commands per object
 *
 * @ingroup Renderer
 */
struct DrawParameters {
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    std::uint32_t indexCount;
    std::uint32_t indexOffset;
    std::uint32_t vertexOffset;
    std::uint32_t firstInstance;
    std::uint32_t instanceCount;
};

} // namespace render
} // namespace bl

#endif
