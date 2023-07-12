#include <BLIB/Render/Resources/PipelineLayoutCache.hpp>

namespace bl
{
namespace rc
{
namespace res
{

PipelineLayoutCache::PipelineLayoutCache(Renderer& renderer)
: renderer(renderer) {}

vk::PipelineLayout* PipelineLayoutCache::getLayout(vk::PipelineLayout::LayoutParams&& params) {
    return &(cache
                 .try_emplace(static_cast<const vk::PipelineLayout::LayoutParams&>(params),
                              renderer,
                              std::forward<vk::PipelineLayout::LayoutParams>(params))
                 .first->second);
}

void PipelineLayoutCache::cleanup() { cache.clear(); }

} // namespace res
} // namespace rc
} // namespace bl
