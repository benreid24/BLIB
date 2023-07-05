#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORCOMPONENTSTORAGECACHE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORCOMPONENTSTORAGECACHE_HPP

#include <BLIB/Render/Descriptors/DescriptorComponentStorage.hpp>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace gfx
{
namespace ds
{
/**
 * @brief Cache for component storage modules for descriptor sets. Storage modules are instantiated
 *        once and shared across descriptor sets
 *
 * @ingroup Renderer
 */
class DescriptorComponentStorageCache {
public:
    /**
     * @brief Creates a new storage cache
     *
     * @param registry The ECS registry instance
     * @param vulkanState The renderer Vulkan state
     * @param entityCb Callback to map scene id to ECS id
     */
    DescriptorComponentStorageCache(ecs::Registry& registry, vk::VulkanState& vulkanState,
                                    const DescriptorComponentStorageBase::EntityCallback& entityCb);

    /**
     * @brief Gets or creates the storage module for the given component type
     *
     * @tparam TCom The component type to get or create the module for
     * @tparam TPayload The underlying descriptor data type
     * @tparam TDynamicStorage The storage for dynamic data
     * @tparam TStaticStorage The storage for static data
     * @return A pointer to the descriptor component module to use
     */
    template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
    DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>*
    getComponentStorage();

    /**
     * @brief Syncs modified descriptor values in the contained storage modules
     */
    void syncDescriptors();

private:
    ecs::Registry& registry;
    vk::VulkanState& vulkanState;
    const DescriptorComponentStorageBase::EntityCallback entityCb;
    std::unordered_map<std::type_index, std::unique_ptr<DescriptorComponentStorageBase>> cache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>*
DescriptorComponentStorageCache::getComponentStorage() {
    using Storage = DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>;

    auto it = cache.find(typeid(TCom));
    if (it == cache.end()) {
        it = cache.try_emplace(typeid(TCom), new Storage(registry, vulkanState, entityCb)).first;
    }
#ifdef BLIB_DEBUG
    Storage* result = dynamic_cast<Storage*>(it->second.get());
    if (!result) {
        throw std::runtime_error("Mismatch of component storages across descriptor sets");
    }
    return result;
#else
    return static_cast<Storage*>(it->second.get());
#endif
}

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
