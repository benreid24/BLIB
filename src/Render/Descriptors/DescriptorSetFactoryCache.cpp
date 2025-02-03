#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
DescriptorSetFactoryCache::DescriptorSetFactoryCache(engine::Engine& engine, Renderer& r)
: engine(engine)
, renderer(r) {}

void DescriptorSetFactoryCache::cleanup() { cache.clear(); }

DescriptorSetFactory* DescriptorSetFactoryCache::getOrAddFactory(
    std::type_index tid, std::unique_ptr<DescriptorSetFactory>&& factory) {
    auto it = cache.find(tid);
    if (it != cache.end()) { return it->second.get(); }
    if (!factory) { throw std::runtime_error("Factory is null and does not already exist"); }
    it = cache.try_emplace(tid, std::move(factory)).first;
    it->second->init(engine, renderer);
    return it->second.get();
}

} // namespace ds
} // namespace rc
} // namespace bl
