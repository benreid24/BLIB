#ifndef BLIB_RENDER_VULKAN_PIPELINELAYOUTCACHE_HPP
#define BLIB_RENDER_VULKAN_PIPELINELAYOUTCACHE_HPP

#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
#include <unordered_map>

namespace bl
{
namespace gfx
{
class Renderer;

namespace res
{
/**
 * @brief Renderer level cache of pipeline layouts
 *
 * @ingroup Renderer
 */
class PipelineLayoutCache {
public:
    /**
     * @brief Creates or fetches a layout that has the given creation parameters
     *
     * @param params Desired pipeline layout
     * @return A pointer to a PipelineLayout with the desired format
     */
    vk::PipelineLayout* getLayout(vk::PipelineLayout::LayoutParams&& params);

private:
    Renderer& renderer;
    std::unordered_map<vk::PipelineLayout::LayoutParams, vk::PipelineLayout> cache;

    PipelineLayoutCache(Renderer& renderer);
    void cleanup();

    friend class Renderer;
};

} // namespace res
} // namespace gfx
} // namespace bl

#endif
