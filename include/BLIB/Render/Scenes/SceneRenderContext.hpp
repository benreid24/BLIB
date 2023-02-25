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
     * @param commandBuffer The command buffer to record commands into
     * @param projView The projection matrix multiplied by the view matrix
     */
    SceneRenderContext(VkCommandBuffer commandBuffer, const glm::mat4& projView);

    const VkCommandBuffer commandBuffer;
    const glm::mat4 projView;
};

} // namespace render
} // namespace bl

#endif
