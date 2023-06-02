#ifndef BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP
#define BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <array>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
namespace scene
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
     * @param observerIndex Index of the observer currently rendering the scene
     */
    SceneRenderContext(VkCommandBuffer commandBuffer, std::uint32_t observerIndex);

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
     * @param descriptorCount The number of descriptors to bind
     */
    void bindDescriptors(VkPipelineLayout layout, ds::DescriptorSetInstance** descriptors,
                         std::uint32_t descriptorCount);

    /**
     * @brief Issues the required commands to render the given scene object
     *
     * @param layout Layout of the currently bound pipeline
     * @param object The object to render
     */
    void renderObject(VkPipelineLayout layout, const SceneObject& object);

private:
    const VkCommandBuffer commandBuffer;
    const std::uint32_t observerIndex;
    VkBuffer prevVB;
    VkBuffer prevIB;
    std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> boundDescriptors;
    std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> perObjDescriptors;
    std::uint32_t perObjStart, perObjCount;
    // TODO - add parent viewport/scissor
};

} // namespace scene
} // namespace render
} // namespace bl

#endif
