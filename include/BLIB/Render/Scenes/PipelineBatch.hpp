#ifndef BLIB_RENDER_RENDERER_PIPELINEBATCH_HPP
#define BLIB_RENDER_RENDERER_PIPELINEBATCH_HPP

#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <mutex>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

/**
 * @brief Concrete instance of a rendering pipeline which exists within a scene. Owns the Objects to
 *        be rendered within it
 *
 * @ingroup Renderer
 */
class PipelineBatch {
public:
    const std::uint32_t pipelineId;

    /**
     * @brief Construct a new Pipeline Instance
     *
     * @param renderer The Renderer the instance belongs to
     * @param maxObjects The maximum number of objects that may be in the scene
     * @param descriptorCache Descriptor set cache to use when allocating descriptor sets
     * @param pipelineId The id of the pipeline to use
     */
    PipelineBatch(Renderer& renderer, std::uint32_t maxObjects,
                  ds::DescriptorSetInstanceCache& descriptorCache, std::uint32_t pipelineId);

    /**
     * @brief Records the commands necessary to render the pipeline and its objects. Called once per
     *        rendered frame
     *
     * @param context Render context containing the parameters to render with
     */
    void recordRenderCommands(SceneRenderContext& context);

    /**
     * @brief Creates a new object to be rendered with the pipeline
     *
     * @param object The object to add to this pipeline batch
     * @param entity The ECS id of the entity of this object
     * @param updateFreq How often the object is expected to be changed
     */
    void addObject(SceneObject* object, ecs::Entity entity, SceneObject::UpdateSpeed updateFreq);

    /**
     * @brief Removes the given object from the render batch. May reorder the remaining objects
     *        unless preserveObjectOrder was true during creation
     *
     * @param object The object to remove
     * @param entity The ECS id of the object to remove
     */
    void removeObject(SceneObject* object, ecs::Entity entity);

private:
    Pipeline& pipeline;
    std::vector<ds::DescriptorSetInstance*> descriptors;
    std::vector<SceneObject*> objects;
};

} // namespace render
} // namespace bl

#endif
