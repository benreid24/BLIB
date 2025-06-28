#ifndef BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP
#define BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Materials/MaterialPipeline.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/RenderPhase.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
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
     * @param renderPhase The current renderer phase
     * @param renderPassId The id of the render pass that is currently active
     * @param renderingToRenderTexture True if target is render texture, false otherwise
     */
    SceneRenderContext(VkCommandBuffer commandBuffer, std::uint32_t observerIndex,
                       const VkViewport& viewport, RenderPhase renderPhase,
                       std::uint32_t renderPassId, bool renderingToRenderTexture);

    /**
     * @brief Binds the given pipeline
     *
     * @param pipeline The pipeline to bind
     * @param specialization The specialization id to use in the pipeline
     */
    void bindPipeline(mat::MaterialPipeline& pipeline, std::uint32_t specialization);

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
     * @brief Issues the required commands to render the given scene object
     *
     * @param object The object to render
     */
    void renderObject(const rcom::DrawableBase& object);

    /**
     * @brief Returns the current render phase
     */
    RenderPhase getRenderPhase() const;

    /**
     * @brief Returns the command buffer to use for rendering
     */
    VkCommandBuffer getCommandBuffer() const;

    /**
     * @brief Returns the viewport the scene is being rendered to
     */
    const VkViewport& parentViewport() const;

    /**
     * @brief Returns the id of the active render pass
     */
    std::uint32_t currentRenderPass() const;

    /**
     * @brief Returns whether or not the current target is a render texture
     */
    bool targetIsRenderTexture() const;

    /**
     * @brief Returns the scene index of the current observer
     */
    std::uint32_t currentObserverIndex() const;

    /**
     * @brief Add extra context to the scene render context
     *
     * @tparam T The type of extra context to add
     * @param ctx Pointer to the extra context. Must remain in scope during render
     */
    template<typename T>
    void setExtraContext(T* ctx);

    /**
     * @brief Returns whether extra context has been set
     */
    bool hasExtraContext() const;

    /**
     * @brief Returns the type of extra context
     */
    std::type_index getExtraContextType() const;

    /**
     * @brief Returns the extra render context. Performs validation and returns nullptr if invalid
     *
     * @tparam T The type of extra context to retrieve
     * @return Pointer to the extra context
     */
    template<typename T>
    T* getExtraContext() const;

private:
    const VkCommandBuffer commandBuffer;
    const std::uint32_t observerIndex;
    const RenderPhase renderPhase;
    VkBuffer prevVB;
    VkBuffer prevIB;
    UpdateSpeed boundSpeed;
    std::array<ds::DescriptorSetInstance*, cfg::Limits::MaxDescriptorSets> boundDescriptors;
    const VkViewport viewport;
    const std::uint32_t renderPassId;
    const bool isRenderTexture;
    std::type_index extraContextType;
    void* extraContext;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkCommandBuffer SceneRenderContext::getCommandBuffer() const { return commandBuffer; }

inline const VkViewport& SceneRenderContext::parentViewport() const { return viewport; }

inline RenderPhase SceneRenderContext::getRenderPhase() const { return renderPhase; }

inline std::uint32_t SceneRenderContext::currentRenderPass() const { return renderPassId; }

inline bool SceneRenderContext::targetIsRenderTexture() const { return isRenderTexture; }

inline std::uint32_t SceneRenderContext::currentObserverIndex() const { return observerIndex; }

inline bool SceneRenderContext::hasExtraContext() const { return extraContext != nullptr; }

inline std::type_index SceneRenderContext::getExtraContextType() const { return extraContextType; }

template<typename T>
void SceneRenderContext::setExtraContext(T* ctx) {
    extraContextType = std::type_index(typeid(T));
    extraContext     = ctx;
}

template<typename T>
T* SceneRenderContext::getExtraContext() const {
    if (!extraContext || extraContextType != std::type_index(typeid(T))) { return nullptr; }
    return static_cast<T*>(extraContext);
}

} // namespace scene
} // namespace rc
} // namespace bl

#endif
