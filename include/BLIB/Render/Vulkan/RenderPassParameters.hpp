#ifndef BLIB_RENDER_VULKAN_RENDERPASSPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_RENDERPASSPARAMETERS_HPP

#include <cstdint>
#include <volk.h>
#include <optional>
#include <vector>

namespace bl
{
namespace gfx
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
         * @brief Performs validation and returns a usable rvalue reference to this object
         *
         * @return SubPass&& An rvalue reference to this object
         */
        SubPass&& build();

    private:
        std::vector<VkAttachmentReference> colorAttachments;
        std::optional<VkAttachmentReference> depthAttachment;

        friend class RenderPass;
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
     * @brief Performs validation and returns a usable rvalue reference to this object
     *
     * @return RenderPassParameters&& An rvalue reference to this object
     */
    RenderPassParameters&& build();

private:
    std::vector<VkAttachmentDescription> attachments;
    std::vector<SubPass> subpasses;
    std::vector<VkSubpassDependency> dependencies;

    friend class RenderPass;
};

} // namespace vk
} // namespace gfx
} // namespace bl

#endif
