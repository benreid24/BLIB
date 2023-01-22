#ifndef BLIB_RENDER_RENDERER_RENDERPASSBATCH_HPP
#define BLIB_RENDER_RENDERER_RENDERPASSBATCH_HPP

#include <BLIB/Render/Renderer/PipelineBatch.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>
#include <BLIB/Render/Util/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

/**
 * @brief Collection of batched pipeline render data for a single render pass. Used by Scene
 *
 * @ingroup Renderer
 */
class RenderPassBatch {
public:
    /**
     * @brief Construct a new Render Pass Batch
     *
     * @param renderer The renderer the pass belongs to
     * @param renderPassId The id of the render pass to use
     */
    RenderPassBatch(Renderer& renderer, std::uint32_t renderPassId);

    /**
     * @brief Creates a new object to be rendered in the pass
     *
     * @param object The object to add
     * @param pipelineId The pipeline to render the object with
     */
    void addObject(const Object::Handle& object, std::uint32_t pipelineId);

    /**
     * @brief Changes which pipeline is used to render the given object
     *
     * @param object The object to swap pipelines for
     * @param oldPipeline The pipeline the object is currently being rendered with
     * @param newPipeline The pipeline the object should be rendered with
     */
    void changePipeline(const Object::Handle& object, std::uint32_t oldPipeline,
                        std::uint32_t newPipeline);

    /**
     * @brief Removes the given object from the render pass
     *
     * @param object The object to remove
     * @param pipelineId The pipeline the object is being rendered with
     */
    void removeObject(const Object::Handle& object, std::uint32_t pipelineId);

    /**
     * @brief Records the commands necessary to complete this render pass. Binds the render pass and
     *        renders all contained objects with their respective pipelines
     *
     * @param commandBuffer The command buffer to record to
     * @param target The target that will be rendered to
     * @param clearColors The colors to clear the framebuffer with initially
     * @param clearColorCount The number of clear colors
     */
    void recordRenderCommands(VkCommandBuffer commandBuffer, const AttachmentSet& target,
                              VkClearValue* clearColors, std::uint32_t clearColorCount);

private:
    Renderer& renderer;
    VkRenderPass renderPass;
    PerSwapFrame<Framebuffer> framebuffers;
    std::vector<PipelineBatch> batches;
};

} // namespace render
} // namespace bl

#endif
