#ifndef BLIB_RENDER_VULKAN_RENDERPASSPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_RENDERPASSPARAMETERS_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Graph/Assets/MSAABehavior.hpp>
#include <BLIB/Render/Vulkan/SemanticTextureFormat.hpp>
#include <BLIB/Vulkan.hpp>
#include <cstdint>
#include <optional>
#include <vector>

namespace bl
{
namespace rc
{
namespace vk
{
class RenderPass;

/**
 * @brief Parameter builder class used to construct RenderPass objects in the renderer
 *
 * @ingroup Renderer
 */
class RenderPassParameters {
public:
    static constexpr std::uint32_t MaxSubpassCount    = 4;
    static constexpr std::uint32_t MaxAttachmentCount = 16;
    static constexpr std::uint32_t MaxDependencyCount = 16;

    /// Controls the MSAA behavior of the render pass
    using MSAABehavior = bl::rc::rgi::MSAABehavior;

    /**
     * @brief Helper parameter class that represents a subpass within a render pass
     *
     * @ingroup Renderer
     */
    class SubPass {
    public:
        /**
         * @brief Construct a new Sub Pass object
         *
         */
        SubPass();

        /**
         * @brief Adds an input attachment reference to the subpass
         *
         * @param index The index of the attachment
         * @param layout The image layout of the attachment
         * @return SubPass& A reference to this object
         */
        SubPass& withInputAttachment(std::uint32_t index, VkImageLayout layout);

        /**
         * @brief Adds an attachment reference to the subpass
         *
         * @param index The index of the attachment
         * @param layout The image layout of the attachment
         * @return SubPass& A reference to this object
         */
        SubPass& withAttachment(std::uint32_t index, VkImageLayout layout);

        /**
         * @brief Adds a depth attachment to this subpass
         *
         * @param index The index of the attachment
         * @param layout The layout of the depth attachment
         * @return A reference to this object
         */
        SubPass& withDepthAttachment(std::uint32_t index, VkImageLayout layout);

        /**
         * @brief Adds an preserved attachment to the subpass
         *
         * @param index The index of the attachment to preserve
         * @return SubPass& A reference to this object
         */
        SubPass& withPreserveAttachment(std::uint32_t index);

        /**
         * @brief Performs validation and returns a usable rvalue reference to this object
         *
         * @return SubPass&& An rvalue reference to this object
         */
        SubPass&& build();

    private:
        ctr::StaticVector<VkAttachmentReference, MaxAttachmentCount> inputAttachments;
        ctr::StaticVector<VkAttachmentReference, MaxAttachmentCount> colorAttachments;
        ctr::StaticVector<std::uint32_t, MaxAttachmentCount> preserveAttachments;
        ctr::StaticVector<VkAttachmentReference, MaxAttachmentCount> resolveAttachments;
        std::optional<VkAttachmentReference> depthAttachment;

        friend class RenderPass;
        friend class RenderPassParameters;
    };

    /**
     * @brief Construct a new Render Pass Parameters object
     *
     */
    RenderPassParameters();

    /**
     * @brief Adds a subpass to the render pass to be created
     *
     * @param subpass The subpass to add
     * @return RenderPassParameters& A reference to this object
     */
    RenderPassParameters& addSubpass(SubPass&& subpass);

    /**
     * @brief Uses the prior pass outputs as the given subpass' inputs. The depth attachment, if
     *        present, will be last
     *
     * @param index The index of the subpass taking the inputs
     * @param includeDepth Whether to include the depth attachment as an input or not
     * @return A reference to this object
     */
    RenderPassParameters& useSubpassOutputsAsInputs(std::uint32_t index, bool includeDepth = true);

    /**
     * @brief Adds a blocking dependency to block the renderer between subpasses
     *
     * @param dependency The dependency to add
     * @return RenderPassParameters& A reference to this object
     */
    RenderPassParameters& addSubpassDependency(VkSubpassDependency dependency);

    /**
     * @brief Adds an image attachment to this render pass
     *
     * @param attachment The image attachment descriptor to add
     * @return RenderPassParameters& A reference to this object
     */
    RenderPassParameters& addAttachment(VkAttachmentDescription attachment);

    /**
     * @brief Replaces an existing attachment at the given index with a new attachment
     *
     * @param i The index of the attachment to replace
     * @param attachment The new attachment descriptor to replace the existing one with
     * @return A reference to this object
     */
    RenderPassParameters& replaceAttachment(std::uint32_t i, VkAttachmentDescription attachment);

    /**
     * @brief Sets a semantic format override for the given attachment. This makes the format of
     *        that attachment dynamic based on the renderer settings
     *
     * @param i The index of the attachment to set the semantic format for
     * @param semanticFormat The semantic format to use
     * @return A reference to this object
     */
    RenderPassParameters& withSemanticAttachmentFormat(std::uint32_t i,
                                                       SemanticTextureFormat semanticFormat);

    /**
     * @brief Sets the MSAA behavior of this render pass. Controls the sample count of color
     *        and depth attachments
     *
     * @param behavior The MSAA behavior to use
     * @return RenderPassParameters& A reference to this object
     */
    RenderPassParameters& withMSAABehavior(MSAABehavior behavior);

    /**
     * @brief Sets a debug name on the render pass
     *
     * @param name The debug name to set
     * @return A reference to this object
     */
    RenderPassParameters& withDebugName(const char* name);

    /**
     * @brief Performs validation and returns a usable rvalue reference to this object
     *
     * @return RenderPassParameters&& An rvalue reference to this object
     */
    RenderPassParameters&& build();

    /**
     * @brief Returns the msaa behavior of this render pass
     */
    MSAABehavior getMSAABehavior() const { return msaaBehavior; }

    /**
     * @brief Returns whether the render pass clears attachments on start
     */
    bool isClearedOnStart() const { return isCleared; }

private:
    ctr::StaticVector<SemanticTextureFormat, MaxAttachmentCount> semanticFormats;
    ctr::StaticVector<VkAttachmentDescription, MaxAttachmentCount> attachments;
    ctr::StaticVector<SubPass, MaxSubpassCount> subpasses;
    ctr::StaticVector<VkSubpassDependency, MaxDependencyCount> dependencies;
    MSAABehavior msaaBehavior;
    bool isCleared;
    const char* debugName;

    friend class RenderPass;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
