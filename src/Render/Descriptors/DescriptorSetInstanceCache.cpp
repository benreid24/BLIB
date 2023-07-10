#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
DescriptorSetInstanceCache::DescriptorSetInstanceCache(
    DescriptorComponentStorageCache& storageCache)
: storageCache(storageCache) {
    sceneSets.reserve(8);
}

DescriptorSetInstance* DescriptorSetInstanceCache::getDescriptorSet(DescriptorSetFactory* factory) {
    auto it = cache.find(factory);
    if (it != cache.end()) { return it->second.get(); }
    it = cache.try_emplace(factory, factory->createDescriptorSet()).first;
    it->second->init(storageCache);
    SceneDescriptorSetInstance* scene = dynamic_cast<SceneDescriptorSetInstance*>(it->second.get());
    if (scene) { sceneSets.push_back(scene); }
    return it->second.get();
}

void DescriptorSetInstanceCache::unlinkSceneObject(ecs::Entity ent, scene::Key key) {
    for (auto& pair : cache) { pair.second->releaseObject(ent, key); }
}

void DescriptorSetInstanceCache::handleDescriptorSync() {
    for (auto& pair : cache) { pair.second->handleFrameStart(); }
}

void DescriptorSetInstanceCache::updateObserverCamera(std::uint32_t observerIndex,
                                                      const glm::mat4& projView) {
    for (auto* set : sceneSets) { set->updateObserverCamera(observerIndex, projView); }
}

} // namespace ds
} // namespace gfx
} // namespace bl
