#ifndef BLIB_RENDER_HEADERHELPERS_HPP
#define BLIB_RENDER_HEADERHELPERS_HPP

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{
struct VulkanLayer;
}

/**
 * @brief Helper struct to get members of Renderer without needing to include Renderer.hpp
 *
 * @ingroup Renderer
 */
struct HeaderHelpers {
    /**
     * @brief Returns the vulkan layer of the given renderer
     *
     * @param renderer The renderer instance
     * @return The Vulkan layer of the renderer
     */
    static vk::VulkanLayer& getVulkanLayer(Renderer& renderer);
};

} // namespace rc
} // namespace bl

#endif
