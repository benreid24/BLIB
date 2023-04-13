#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>

namespace bl
{
namespace render
{
namespace ds
{
DescriptorSetInstanceCache::DescriptorSetInstanceCache(std::uint32_t ms, std::uint32_t md)
: maxStatic(ms)
, maxDynamic(md) {}

DescriptorSetInstance* DescriptorSetInstanceCache::getDescriptorSet(DescriptorSetFactory* factory) {
    auto it = cache.find(factory);
    if (it != cache.end()) { return it->second.get(); }
    it = cache.try_emplace(factory, std::move(factory->createDescriptorSet())).first;
    it->second->init(maxStatic, maxDynamic);
    return it->second.get();
}

void DescriptorSetInstanceCache::unlinkSceneObject(std::uint32_t sceneId, ecs::Entity ent) {
    for (auto& pair : cache) { pair.second->releaseObject(sceneId, ent); }
}

void DescriptorSetInstanceCache::handleDescriptorSync() {
    for (auto& pair : cache) { pair.second->handleFrameStart(); }
}

} // namespace ds
} // namespace render
} // namespace bl
