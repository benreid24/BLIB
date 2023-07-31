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
    /**
     * @brief Represents the drawing command to use
     */
    enum struct DrawType { VertexBuffer, IndexBuffer };

    DrawType type;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    union {
        std::uint32_t indexCount;
        std::uint32_t vertexCount;
    };
    std::uint32_t indexOffset;
    std::uint32_t vertexOffset;
    std::uint32_t firstInstance;
    std::uint32_t instanceCount;

    /**
     * @brief Initializes the draw parameters to reasonable defaults
     */
    DrawParameters()
    : type(DrawType::IndexBuffer)
    , vertexBuffer(nullptr)
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
