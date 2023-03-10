#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>

namespace bl
{
namespace render
{
namespace ds
{
DescriptorSetFactoryCache::DescriptorSetFactoryCache(engine::Engine& engine, Renderer& r)
: engine(engine)
, renderer(r) {}

void DescriptorSetFactoryCache::cleanup() { cache.clear(); }

DescriptorSetFactory* DescriptorSetFactoryCache::getFactory(
    std::type_index tid, std::unique_ptr<DescriptorSetFactory>&& factory) {
    auto it = cache.find(tid);
    if (it != cache.end()) { return it->second.get(); }
    it = cache.try_emplace(tid, std::move(factory)).first;
    it->second->init(engine, renderer);
    return it->second.get();
}

} // namespace ds
} // namespace render
} // namespace bl
