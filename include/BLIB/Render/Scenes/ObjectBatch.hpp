#ifndef BLIB_RENDER_RENDERER_OBJECTBATCH_HPP
#define BLIB_RENDER_RENDERER_OBJECTBATCH_HPP

#include <BLIB/Render/Scenes/LayoutBatch.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <vector>

namespace bl
{
namespace gfx
{
class Renderer;

namespace scene
{
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
     * @param maxObjects The maximum number of objects in the scene
     * @param descriptorCache Descriptor set cache for the scene this stage is a member of
     */
    ObjectBatch(Renderer& renderer, std::uint32_t maxObjects,
                ds::DescriptorSetInstanceCache& descriptorCache);

    /**
     * @brief Creates a new object to be rendered in the pass
     *
     * @param object The object to add
     * @param pipelineId The pipeline to render the object with
     * @param entity The ECS id of the entity of this object
     * @param updateFreq How often the object is expected to be changed
     * @return Whether or not the object could be added
     */
    bool addObject(SceneObject* object, std::uint32_t pipelineId, ecs::Entity entity,
                   UpdateSpeed updateFreq);

    /**
     * @brief Removes the given object from the render pass
     *
     * @param object The object to remove
     * @param pipelineId The pipeline the object is being rendered with
     * @param entity The ECS id of the object to remove
     */
    void removeObject(SceneObject* object, std::uint32_t pipelineId, ecs::Entity entity);

    /**
     * @brief Records the commands necessary to complete this render pass. Binds the render pass and
     *        renders all contained objects with their respective pipelines
     *
     * @param context Rendering context containing parameters for the render
     */
    void recordRenderCommands(SceneRenderContext& context);

private:
    const std::uint32_t maxObjects;
    Renderer& renderer;
    ds::DescriptorSetInstanceCache& descriptorCache;
    std::vector<LayoutBatch> batches;
};

} // namespace scene
} // namespace gfx
} // namespace bl

#endif
