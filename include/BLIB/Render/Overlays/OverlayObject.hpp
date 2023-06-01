#ifndef BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
namespace ovy
{
struct OverlayObject {
    /**
     * @brief Construct a new OverlayObject
     */
    OverlayObject();

    bool hidden;
    std::uint32_t overlayId;
    std::uint32_t parentId;
    std::vector<std::uint32_t> children;

    // TODO - viewport/scissor

    VkPipeline pipeline;
    std::array<ds::DescriptorSetInstance*> descriptors;
    std::uint8_t descriptorCount;
    prim::DrawParameters drawParams;
};

} // namespace ovy
} // namespace render
} // namespace bl

#endif
