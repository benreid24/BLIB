#ifndef BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP

#include <BLIB/Components/OverlayScaler.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/ECS/Traits/ParentAware.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <cstdint>
#include <vector>

namespace bl
{
namespace rc
{
class Overlay;

/// Classes for Observer overlays
namespace ovy
{
struct OverlayObject
: public scene::SceneObject
, public ecs::trait::ParentAware<OverlayObject>
, public ecs::trait::ChildAware<OverlayObject> {
    /**
     * @brief Construct a new OverlayObject
     */
    OverlayObject();

    /**
     * @brief Recomputes the target-space viewport
     *
     * @param viewport Pointer to this object's viewport component, if any
     * @param overlay The top-level viewport of the entire overlay
     * @param parent The parent viewport of this object
     */
    void refreshViewport(Viewport* viewport, const VkViewport& overlay, const VkViewport& parent);

    /**
     * @brief Issues the commands to set the viewport and scissor
     *
     * @param commandBuffer The command buffer to issue commands into
     */
    void applyViewport(VkCommandBuffer commandBuffer);

    ecs::Entity entity;
    Overlay* overlay;
    vk::Pipeline* pipeline;
    std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> descriptors;
    std::uint8_t descriptorCount;
    std::uint8_t perObjStart;

    VkViewport cachedViewport;
    VkRect2D cachedScissor;
};

} // namespace ovy
} // namespace rc
} // namespace bl

#endif
