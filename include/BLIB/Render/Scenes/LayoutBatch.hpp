#ifndef BLIB_RENDER_SCENES_LAYOUTBATCH_HPP
#define BLIB_RENDER_SCENES_LAYOUTBATCH_HPP

#include <BLIB/Render/Scenes/PipelineBatch.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
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
 * @brief Collection of objects whose pipelines all share the same descriptor layout
 *
 * @ingroup Renderer
 */
class LayoutBatch {
public:
    /**
     * @brief Creates an empty batch for the given layout
     *
     * @param renderer The renderer instance
     * @param maxObjects The maximum amount of objects in the scene
     * @param descriptorCache The descriptor set instance cache of the scene
     * @param layout The layout to batch for
     */
    LayoutBatch(Renderer& renderer, std::uint32_t maxObjects,
                ds::DescriptorSetInstanceCache& descriptorCache, const vk::PipelineLayout& layout);

    /**
     * @brief Records the commands necessary to render the objects
     *
     * @param context Render context containing the parameters to render with
     */
    void recordRenderCommands(SceneRenderContext& context);

    /**
     * @brief Creates a new object to be rendered with the pipeline
     *
     * @param object The object to add to this pipeline batch
     * @param entity The ECS id of the entity of this object
     * @param pipeline The pipeline to render the object with
     * @param updateFreq How often the object is expected to be changed
     * @return Whether or not the object could be added
     */
    bool addObject(SceneObject* object, ecs::Entity entity, vk::Pipeline& pipeline,
                   UpdateSpeed updateFreq);

    /**
     * @brief Removes the given object from the render batch
     *
     * @param object The object to remove
     * @param entity The ECS id of the object to remove
     * @param pipelineId The pipeline the object is being rendered with
     */
    void removeObject(SceneObject* object, ecs::Entity entity, std::uint32_t pipelineId);

    /**
     * @brief Returns the pipeline layout of this batch
     */
    constexpr VkPipelineLayout getLayout() const;

private:
    Renderer& renderer;
    const std::uint32_t maxObjects;
    const vk::PipelineLayout& layout;
    std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> descriptors;
    std::uint8_t descriptorCount;
    std::uint8_t perObjStart;
    std::vector<PipelineBatch> batches;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkPipelineLayout LayoutBatch::getLayout() const { return layout.rawLayout(); }

} // namespace scene
} // namespace gfx
} // namespace bl

#endif
