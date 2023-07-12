#ifndef BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP
#define BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
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
     * @param viewport The current viewport
     * @param renderPassId The id of the render pass that is currently active
     * @param renderingToRenderTexture True if target is render texture, false otherwise
     */
    SceneRenderContext(VkCommandBuffer commandBuffer, std::uint32_t observerIndex,
                       const VkViewport& viewport, std::uint32_t renderPassId,
                       bool renderingToRenderTexture);

    /**
     * @brief Binds the given pipeline
     *
     * @param pipeline The pipeline to bind
     */
    void bindPipeline(vk::Pipeline& pipeline);

    /**
     * @brief Binds the given descriptors. Only issues bind commands for descriptors that changed
     *        from the last call to bindDescriptors()
     *
     * @param layout Layout of the pipeline the descriptors are being bound to
     * @param updateFreq The update speed of the descriptors to bind
     * @param descriptors The list of descriptor sets to bind
     * @param descriptorCount The number of descriptors to bind
     */
    void bindDescriptors(VkPipelineLayout layout, UpdateSpeed updateFreq,
                         ds::DescriptorSetInstance** descriptors, std::uint32_t descriptorCount);

    /**
     * @brief Issues the required commands to render the given scene object
     *
     * @param object The object to render
     */
    void renderObject(const SceneObject& object);

    /**
     * @brief Returns the command buffer to use for rendering
     */
    constexpr VkCommandBuffer getCommandBuffer() const;

    /**
     * @brief Returns the viewport the scene is being rendered to
     */
    constexpr const VkViewport& parentViewport() const;

    /**
     * @brief Returns the id of the active render pass
     */
    constexpr std::uint32_t currentRenderPass() const;

    /**
     * @brief Returns whether or not the current target is a render texture
     */
    constexpr bool targetIsRenderTexture() const;

    /**
     * @brief Returns the scene index of the current observer
     */
    constexpr std::uint32_t currentObserverIndex() const;

private:
    const VkCommandBuffer commandBuffer;
    const std::uint32_t observerIndex;
    VkBuffer prevVB;
    VkBuffer prevIB;
    UpdateSpeed boundSpeed;
    std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> boundDescriptors;
    const VkViewport viewport;
    const std::uint32_t renderPassId;
    const bool isRenderTexture;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkCommandBuffer SceneRenderContext::getCommandBuffer() const {
    return commandBuffer;
}

inline constexpr const VkViewport& SceneRenderContext::parentViewport() const { return viewport; }

inline constexpr std::uint32_t SceneRenderContext::currentRenderPass() const {
    return renderPassId;
}

inline constexpr bool SceneRenderContext::targetIsRenderTexture() const { return isRenderTexture; }

inline constexpr std::uint32_t SceneRenderContext::currentObserverIndex() const {
    return observerIndex;
}

} // namespace scene
} // namespace rc
} // namespace bl

#endif
