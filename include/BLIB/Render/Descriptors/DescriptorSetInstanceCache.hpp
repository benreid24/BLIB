#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCECACHE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCECACHE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/ShaderResources/ShaderResourceStore.hpp>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Utility class to ensure that descriptor sets are created only once across a set of
 *        pipelines present in a particular scene. Owned by Scene and used by PipelineBatch
 *
 * @ingroup Renderer
 */
class DescriptorSetInstanceCache {
public:
    /**
     * @brief Creates the descriptor set cache
     *
     * @param ctx Context containing all data required to create and initialize descriptor sets
     */
    DescriptorSetInstanceCache(const InitContext& ctx);

    /**
     * @brief Fetches or creates a descriptor set for the given descriptor set factory
     *
     * @param factory The factory to get or create a set instance for
     * @return The descriptor set instance to use
     */
    DescriptorSetInstance* getDescriptorSet(DescriptorSetFactory* factory);

    /**
     * @brief Goes through all descriptor set instances and calls removeObject for the given object
     *
     * @param entity The ECS id of the object to unlink
     * @param key The key of the object within the scene it belongs to
     */
    void unlinkSceneObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Called once prior to TransferEngine kicking off
     */
    void updateDescriptors();

    /**
     * @brief Fetches the contained descriptor set by type, if present
     *
     * @tparam T The type of descriptor set to fetch
     * @return The descriptor set of the given type, if present
     */
    template<typename T>
    T* getDescriptorSet();

private:
    InitContext ctx;
    std::unordered_map<DescriptorSetFactory*, std::unique_ptr<DescriptorSetInstance>> cache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
T* DescriptorSetInstanceCache::getDescriptorSet() {
    for (auto& p : cache) {
        T* val = dynamic_cast<T*>(p.second.get());
        if (val) { return val; }
    }
    return nullptr;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
