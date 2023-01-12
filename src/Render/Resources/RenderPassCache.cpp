#include <BLIB/Render/Resources/RenderPassCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
RenderPassCache::RenderPassCache(Renderer& r)
: renderer(r) {}

RenderPass& RenderPassCache::createRenderPass(std::uint32_t id, RenderPassParameters&& params) {
    const auto insertResult =
        cache.try_emplace(id, renderer.vulkanState(), std::forward<RenderPassParameters>(params));
    if (!insertResult.second) {
        BL_LOG_WARN << "Duplicate creation of render pass with id: " << id;
    }
    return insertResult.first->second;
}

RenderPass& RenderPassCache::getRenderPass(std::uint32_t id) {
    auto it = cache.find(id);
    if (it == cache.end()) {
        BL_LOG_CRITICAL << "Failed to find render pass with id: " << id;
        throw std::runtime_error("Failed to find render pass");
    }
    return it->second;
}

} // namespace render
} // namespace bl
