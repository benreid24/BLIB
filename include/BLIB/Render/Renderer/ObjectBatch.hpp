#ifndef BLIB_RENDER_RENDERER_OBJECTBATCH_HPP
#define BLIB_RENDER_RENDERER_OBJECTBATCH_HPP

#include <BLIB/Render/Renderer/PipelineBatch.hpp>
#include <BLIB/Render/Util/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

/**
 * @brief Collection of objects to be rendered, batched per pipeline. ObjectBatches should exist
 *        entirely within a subpass of a render pass
 *
 * @ingroup Renderer
 */
class ObjectBatch {
public:
    /**
     * @brief Construct a new ObjectBatch
     *
     * @param renderer The renderer the pass belongs to
     */
    ObjectBatch(Renderer& renderer);

    /**
     * @brief Creates a new object to be rendered in the pass
     *
     * @param object The object to add
     * @param pipelineId The pipeline to render the object with
     */
    void addObject(const SceneObject::Handle& object, std::uint32_t pipelineId);

    /**
     * @brief Changes which pipeline is used to render the given object
     *
     * @param object The object to swap pipelines for
     * @param oldPipeline The pipeline the object is currently being rendered with
     * @param newPipeline The pipeline the object should be rendered with
     */
    void changePipeline(const SceneObject::Handle& object, std::uint32_t oldPipeline,
                        std::uint32_t newPipeline);

    /**
     * @brief Removes the given object from the render pass
     *
     * @param object The object to remove
     * @param pipelineId The pipeline the object is being rendered with
     */
    void removeObject(const SceneObject::Handle& object, std::uint32_t pipelineId);

    /**
     * @brief Records the commands necessary to complete this render pass. Binds the render pass and
     *        renders all contained objects with their respective pipelines
     *
     * @param commandBuffer The command buffer to record to
     */
    void recordRenderCommands(VkCommandBuffer commandBuffer);

private:
    Renderer& renderer;
    std::vector<PipelineBatch> batches;
};

} // namespace render
} // namespace bl

#endif
