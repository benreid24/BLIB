#ifndef BLIB_RENDER_RESOURCES_RENDERPASSCACHE_HPP
#define BLIB_RENDER_RESOURCES_RENDERPASSCACHE_HPP

#include <BLIB/Render/Vulkan/RenderPass.hpp>
#include <cstdint>
#include <unordered_map>

namespace bl
{
namespace render
{
class Renderer;
namespace res
{

/**
 * @brief Basic cache to manage render passes. Stores them and provides access. Owned by the
 *        renderer
 *
 * @ingroup Renderer
 */
class RenderPassCache {
public:
    /**
     * @brief Creates and returns a render pass with the given id and parameters
     *
     * @param renderPassId The id of the new pass to create. Should be unique
     * @param params The parameters to create the pass with
     * @return RenderPass& The new render pass
     */
    vk::RenderPass& createRenderPass(std::uint32_t renderPassId, vk::RenderPassParameters&& params);

    /**
     * @brief Fetches the render pass with the given id. Throws an exception if the id does not
     *        exist yet. Render passes should be created on application startup
     *
     * @param renderPassId The id of the render pass to fetch
     * @return RenderPass& The render pass with the given id
     */
    vk::RenderPass& getRenderPass(std::uint32_t renderPassId);

private:
    Renderer& renderer;
    std::unordered_map<std::uint32_t, vk::RenderPass> cache;

    RenderPassCache(Renderer& renderer);
    void cleanup();
    void addDefaults();

    friend class Renderer;
};

} // namespace res
} // namespace render
} // namespace bl

#endif
