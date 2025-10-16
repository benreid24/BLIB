#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
DescriptorSetInstanceCache::DescriptorSetInstanceCache(
    sr::ShaderResourceStore& globalShaderResources,
    sr::ShaderResourceStore& observerShaderResources, sr::ShaderResourceStore& sceneShaderResources)
: globalShaderResources(globalShaderResources)
, observerShaderResources(observerShaderResources)
, sceneShaderResources(sceneShaderResources) {}

DescriptorSetInstance* DescriptorSetInstanceCache::getDescriptorSet(DescriptorSetFactory* factory) {
    if (!factory->isAutoConstructable()) { return nullptr; }

    auto it = cache.find(factory);
    if (it != cache.end()) { return it->second.get(); }
    it = cache.try_emplace(factory, factory->createDescriptorSet()).first;
    it->second->init(globalShaderResources, sceneShaderResources, observerShaderResources);

    return it->second.get();
}

void DescriptorSetInstanceCache::unlinkSceneObject(ecs::Entity ent, scene::Key key) {
    for (auto& pair : cache) { pair.second->releaseObject(ent, key); }
}

void DescriptorSetInstanceCache::updateDescriptors() {
    for (auto& pair : cache) { pair.second->updateDescriptors(); }
}

} // namespace ds
} // namespace rc
} // namespace bl
