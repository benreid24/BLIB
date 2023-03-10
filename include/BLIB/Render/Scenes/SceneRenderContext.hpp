#ifndef BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP
#define BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Scenes/DrawParameters.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <array>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
/**
 * @brief Container class that holds the required parameters to render a scene and provides methods
 *        for descriptor set management and object rendering
 *
 * @ingroup Renderer
 */
class SceneRenderContext {
public:
    /**
     * @brief Creates a scene render context
     *
     * @param commandBuffer The command buffer to record commands into
     * @param projView The projection matrix multiplied by the view matrix
     */
    SceneRenderContext(VkCommandBuffer commandBuffer, const glm::mat4& projView);

    /**
     * @brief Binds the given pipeline
     *
     * @param pipeline The pipeline to bind
     */
    void bindPipeline(VkPipeline pipeline);

    /**
     * @brief Binds the given descriptors. Only issues bind commands for descriptors that changed
     *        from the last call to bindDescriptors()
     *
     * @param layout Layout of the pipeline the descriptors are being bound to
     * @param descriptors The list of descriptor sets to bind
     */
    void bindDescriptors(VkPipelineLayout layout,
                         const std::vector<ds::DescriptorSetInstance*>& descriptors);

    /**
     * @brief Issues the required commands to render the given scene object
     *
     * @param layout Layout of the currently bound pipeline
     * @param object The object to render
     */
    void renderObject(VkPipelineLayout layout, const SceneObject& object);

private:
    const VkCommandBuffer commandBuffer;
    const glm::mat4& projView;
    VkBuffer prevVB;
    VkBuffer prevIB;
    std::array<ds::DescriptorSetInstance*, 4> boundDescriptors;
    std::array<ds::DescriptorSetInstance*, 4> perObjDescriptors;
    std::uint32_t perObjStart, perObjCount;
};

} // namespace render
} // namespace bl

#endif
