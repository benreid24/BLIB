#ifndef BLIB_RENDER_RESOURCES_RENDERPASSCACHE_HPP
#define BLIB_RENDER_RESOURCES_RENDERPASSCACHE_HPP

#include <BLIB/Render/Renderer/RenderPass.hpp>
#include <unordered_map>

namespace bl
{
namespace render
{
class Renderer;

class RenderPassCache {
public:
    RenderPassCache(Renderer& renderer);

    bool createRenderPass(std::uint32_t renderPassId, RenderPassParameters&& params);

    RenderPass* getRenderPass(std::uint32_t renderPassId);

private:
    Renderer& renderer;
    std::unordered_map<std::uint32_t, RenderPass> cache;
};

} // namespace render
} // namespace bl

#endif
