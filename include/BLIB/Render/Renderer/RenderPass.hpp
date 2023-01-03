#ifndef BLIB_RENDER_RENDERER_RENDERPASS_HPP
#define BLIB_RENDER_RENDERER_RENDERPASS_HPP

#include <BLIB/Render/Renderer/RenderPassParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
class RenderPassCache;

/**
 * @brief Represents a render pass in the renderer. A scene is comprised of a sequence of render
 *        passes and each render pass is comprised of a set of Pipelines and their renderables
 *
 * @ingroup Renderer
 */
class RenderPass {
public:
    /**
     * @brief Destroy the Render Pass object
     *
     */
    ~RenderPass();

    /**
     * @brief Returns the Vulkan render pass handle
     *
     * @return constexpr VkRenderPass The Vulkan render pass handle
     */
    constexpr VkRenderPass rawPass();

private:
    VulkanState& vulkanState;
    VkRenderPass renderPass;

    RenderPass(VulkanState& vulkanState, RenderPassParameters&& params);

    friend class RenderPassCache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

constexpr VkRenderPass RenderPass::rawPass() { return renderPass; }

} // namespace render
} // namespace bl

#endif
