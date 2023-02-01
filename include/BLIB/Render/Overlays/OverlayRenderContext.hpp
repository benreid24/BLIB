#ifndef BLIB_RENDER_OVERLAYS_OVERLAYRENDERCONTEXT_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAYRENDERCONTEXT_HPP

#include <glad/vulkan.h>

namespace bl
{
namespace render
{
namespace overlay
{
/**
 * @brief Contains settings passed to Drawables during overlay rendering
 *
 * @ingroup Renderer
 */
class OverlayRenderContext {
public:
    /**
     * @brief Creates a new render context
     *
     * @param commandBuffer The command buffer to issue draw commands into
     */
    OverlayRenderContext(VkCommandBuffer commandBuffer);

    /**
     * @brief Helper method to bind a pipeline for rendering. This should preferred to manually
     *        binding as it will only bind the pipeline if the new pipeline is different from the
     *        currently bound pipeline, potentially saving some time
     *
     * @param pipeline The pipeline to bind
     */
    void bindPipeline(VkPipeline pipeline);

    const VkCommandBuffer commandBuffer;

private:
    VkPipeline currentPipeline;
};

} // namespace overlay
} // namespace render
} // namespace bl

#endif
