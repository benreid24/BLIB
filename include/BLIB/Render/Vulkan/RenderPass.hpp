#ifndef BLIB_RENDER_VULKAN_RENDERPASS_HPP
#define BLIB_RENDER_VULKAN_RENDERPASS_HPP

#include <BLIB/Render/Vulkan/RenderPassParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Represents a render pass in the renderer. A scene is comprised of a sequence of render
 *        passes and each render pass is comprised of a set of Pipelines and their renderables
 *
 * @ingroup Renderer
 */
class RenderPass {
public:
    /**
     * @brief Construct a new Render Pass
     *
     * @param vulkanState The renderer Vulkan state
     * @param params The parameters to create the pass with
     */
    RenderPass(VulkanState& vulkanState, RenderPassParameters&& params);

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
    constexpr VkRenderPass rawPass() const;

private:
    VulkanState& vulkanState;
    VkRenderPass renderPass;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

constexpr VkRenderPass RenderPass::rawPass() const { return renderPass; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
