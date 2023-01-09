#ifndef BLIB_RENDER_RESOURCES_RENDERPASSCACHE_HPP
#define BLIB_RENDER_RESOURCES_RENDERPASSCACHE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer/RenderPass.hpp>
#include <array>

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
    std::array<RenderPass, Config::RenderPassIds::Count> cache;
};

} // namespace render
} // namespace bl

#endif
