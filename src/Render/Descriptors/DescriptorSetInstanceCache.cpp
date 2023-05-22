#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>

namespace bl
{
namespace render
{
namespace ds
{
DescriptorSetInstanceCache::DescriptorSetInstanceCache(std::uint32_t ms, std::uint32_t md)
: maxStatic(ms)
, maxDynamic(md) {
    sceneSets.reserve(4);
}

DescriptorSetInstance* DescriptorSetInstanceCache::getDescriptorSet(DescriptorSetFactory* factory) {
    auto it = cache.find(factory);
    if (it != cache.end()) { return it->second.get(); }
    it = cache.try_emplace(factory, std::move(factory->createDescriptorSet())).first;
    it->second->init(maxStatic, maxDynamic);
    SceneDescriptorSetInstance* scene = dynamic_cast<SceneDescriptorSetInstance*>(it->second.get());
    if (scene) { sceneSets.push_back(scene); }
    return it->second.get();
}

void DescriptorSetInstanceCache::unlinkSceneObject(std::uint32_t sceneId, ecs::Entity ent) {
    for (auto& pair : cache) { pair.second->releaseObject(sceneId, ent); }
}

void DescriptorSetInstanceCache::handleDescriptorSync() {
    for (auto& pair : cache) { pair.second->handleFrameStart(); }
}

void DescriptorSetInstanceCache::updateObserverCamera(std::uint32_t observerIndex,
                                                      const glm::mat4& projView) {
    for (auto* set : sceneSets) { set->updateObserverCamera(observerIndex, projView); }
}

} // namespace ds
} // namespace render
} // namespace bl
