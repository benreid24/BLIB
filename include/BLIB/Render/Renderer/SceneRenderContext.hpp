#ifndef BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP
#define BLIB_RENDER_RENDERER_SCENERENDERCONTEXT_HPP

#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Render/Vulkan/Viewport.hpp>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
/**
 * @brief Container struct that holds the required parameters to render a scene
 * 
 * @ingroup Renderer
*/
struct SceneRenderContext {
    /**
     * @brief Creates a scene render context
     * 
     * @param target The target to render to
     * @param commandBuffer The command buffer to record commands into
     * @param projView The projection matrix multiplied by the view matrix
     * @param renderRegion Scissor of the area being rendered to
    */
    SceneRenderContext(const StandardAttachmentSet& target, VkCommandBuffer commandBuffer,
                       const glm::mat4& projView, const VkRect2D& renderRegion);

    const VkCommandBuffer commandBuffer;
    const StandardAttachmentSet& target;
    const glm::mat4 projView;
    const VkRect2D& renderRegion;
};

} // namespace render
} // namespace bl

#endif
