#ifndef BLIB_RENDER_OVERLAYS_VIEWPORT_HPP
#define BLIB_RENDER_OVERLAYS_VIEWPORT_HPP

#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
namespace ovy
{
class Viewport {
public:
    Viewport(const VkViewport& viewport);

    Viewport createDerived(const glm::vec4& ratios);

private:
    VkViewport viewport;
    // TODO - scissor
};

} // namespace ovy
} // namespace render
} // namespace bl

#endif
