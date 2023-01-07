#ifndef BLIB_RENDER_RENDERER_PIPELINEINSTANCE_HPP
#define BLIB_RENDER_RENDERER_PIPELINEINSTANCE_HPP

#include <BLIB/Render/Renderer/Object.hpp>
#include <BLIB/Render/Renderer/Pipeline.hpp>
#include <BLIB/Render/Renderer/PipelineParameters.hpp>
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
class PipelineInstance {
public:
    /**
     * @brief Construct a new Pipeline Instance
     *
     * @param renderer The Renderer the instance belongs to
     * @param pipelineId The id of the pipeline to use
     * @param preserveObjectOrder True to keep objects in the order in which they are added
     */
    PipelineInstance(Renderer& renderer, std::uint32_t pipelineId, bool preserveObjectOrder);

    /**
     * @brief Releases resources
     *
     */
    ~PipelineInstance();

    /**
     * @brief Records the commands necessary to render the pipeline and its objects. Called once per
     *        rendered frame
     *
     * @param commandBuffer The primary command buffer to render to
     */
    void recordRenderCommands(VkCommandBuffer commandBuffer);

    /**
     * @brief Creates a new object to be rendered with the pipeline
     *
     * @param owner The Renderable which will own the Object
     * @return Object* The newly created object
     */
    Object* createAndAddObject(Renderable* owner);

    /**
     * @brief Removes the given object from the render batch. May reorder the remaining objects
     *        unless preserveObjectOrder was true during creation
     *
     * @param object The object to remove
     */
    void removeObject(Object* object);

private:
    VulkanState& vulkanState;
    const bool preserveObjectOrder;
    Pipeline* pipeline;
    std::vector<Object> objects;
    std::mutex mutex;
};

} // namespace render
} // namespace bl

#endif
