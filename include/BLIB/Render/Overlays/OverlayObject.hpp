#ifndef BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Render/Components/OverlayScaler.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace gfx
{
/// Classes for Observer overlays
namespace ovy
{
struct OverlayObject : public scene::SceneObject {
    /**
     * @brief Construct a new OverlayObject
     */
    OverlayObject();

    /**
     * @brief Adds the child to this object's child array
     *
     * @param childId The id of the child to add
     */
    void registerChild(std::uint32_t childId);

    /**
     * @brief Removes the given child from this object's child array
     *
     * @param childId The child id to remove
     */
    void removeChild(std::uint32_t childId);

    /**
     * @brief Recomputes the target-space viewport
     *
     * @param overlay The top-level viewport of the entire overlay
     * @param parent The parent viewport of this object
     */
    void refreshViewport(const VkViewport& overlay, const VkViewport& parent);

    /**
     * @brief Issues the commands to set the viewport and scissor
     *
     * @param commandBuffer The command buffer to issue commands into
     */
    void applyViewport(VkCommandBuffer commandBuffer);

    std::vector<std::uint32_t> children;
    ecs::StableHandle<com::OverlayScaler> scaler;
    ecs::StableHandle<Viewport> viewport;
    vk::Pipeline* pipeline;
    std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> descriptors;
    std::uint8_t descriptorCount;

    VkViewport cachedViewport;
    VkRect2D cachedScissor;
};

} // namespace ovy
} // namespace gfx
} // namespace bl

#endif
