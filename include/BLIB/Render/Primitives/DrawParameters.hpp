#ifndef BLIB_RENDER_RENDERER_DRAWPARAMETERS_HPP
#define BLIB_RENDER_RENDERER_DRAWPARAMETERS_HPP

#include <BLIB/Vulkan.hpp>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace prim
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

    /**
     * @brief Initializes the draw parameters to reasonable defaults
     */
    DrawParameters()
    : vertexBuffer(nullptr)
    , indexBuffer(nullptr)
    , indexCount(0)
    , indexOffset(0)
    , vertexOffset(0)
    , firstInstance(0)
    , instanceCount(1) {}
};

} // namespace prim
} // namespace rc
} // namespace bl

#endif
