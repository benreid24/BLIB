#ifndef BLIB_RENDER_RENDERCONTEXT_HPP
#define BLIB_RENDER_RENDERCONTEXT_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <BLIB/Render/Vulkan/Viewport.hpp>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
class Renderer;

class RenderContext {
public:
    // TODO

private:
    const AttachmentSet& target;
    VkCommandBuffer commandBuffer;
    Viewport viewport;
    glm::mat4 viewMatrix;
    glm::mat4 cachedTransform;

    RenderContext(const AttachmentSet& target, VkCommandBuffer commandBuffer,
                  const VkRect2D& initialArea);

    friend class Renderer;
};

} // namespace render
} // namespace bl

#endif
