#include <BLIB/Render/Descriptors/DescriptorComponentStorageCache.hpp>

namespace bl
{
namespace gfx
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

} // namespace ds
} // namespace gfx
} // namespace bl
