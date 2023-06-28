#ifndef BLIB_RENDER_RENDERER_PIPELINEBATCH_HPP
#define BLIB_RENDER_RENDERER_PIPELINEBATCH_HPP

#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
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
 * @brief Concrete instance of a rendering pipeline which exists within a scene. Owns the Objects to
 *        be rendered within it
 *
 * @ingroup Renderer
 */
class PipelineBatch {
public:
    /**
     * @brief Construct a new Pipeline Instance
     *
     * @param maxObjects The maximum number of objects that may be in the scene
     * @param pipeline The id of the pipeline to use
     * @param Pointer to the array of per-object descriptors
     * @param descriptorCount The number of descriptors to bind per-object
     */
    PipelineBatch(std::uint32_t maxObjects, vk::Pipeline& pipeline,
                  ds::DescriptorSetInstance** descriptors, std::uint8_t descriptorCount);

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
     */
    void addObject(SceneObject* object);

    /**
     * @brief Removes the given object from the render batch
     *
     * @param object The object to remove
     */
    void removeObject(SceneObject* object);

    /**
     * @brief Returns whether or not this batch is for the given pipeline
     *
     * @param pipeline The pipeline to test for
     * @return True if the pipeline is the same, false otherwise
     */
    constexpr bool matches(vk::Pipeline& pipeline) const;

private:
    vk::Pipeline& pipeline;
    ds::DescriptorSetInstance** descriptors;
    const std::uint8_t descriptorCount;
    std::vector<SceneObject*> objects;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr bool PipelineBatch::matches(vk::Pipeline& p) const { return &p == &pipeline; }

} // namespace scene
} // namespace gfx
} // namespace bl

#endif
