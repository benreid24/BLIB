#ifndef BLIB_RENDER_VULKAN_VIEWPORT_HPP
#define BLIB_RENDER_VULKAN_VIEWPORT_HPP

#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
class Framebuffer;

class Viewport {
public:
    Viewport();

    void apply(VkCommandBuffer commandBuffer) const;

private:
    glm::mat4 proj; // initialized by camera controller with viewport as param
    VkViewport viewport;
    VkRect2D scissor;

    friend class Framebuffer;
};

} // namespace render
} // namespace bl

#endif
