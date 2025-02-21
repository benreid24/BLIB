#include <BLIB/Render/Descriptors/DescriptorComponentStorageCache.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
DescriptorComponentStorageCache::DescriptorComponentStorageCache(
    ecs::Registry& registry, vk::VulkanState& vulkanState,
    const DescriptorComponentStorageBase::EntityCallback& entityCb)
: registry(registry)
, vulkanState(vulkanState)
, entityCb(entityCb) {}

void DescriptorComponentStorageCache::syncDescriptors() {
    for (auto& pair : cache) { pair.second->performSync(); }
}

void DescriptorComponentStorageCache::copyFromECS() {
    for (auto& pair : cache) { pair.second->copyFromECS(); }
}

} // namespace ds
} // namespace rc
} // namespace bl
