#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCECACHE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCECACHE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
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
     * @param maxStatic The maximum number of static objects contained sets should allow
     * @param maxDynamic The maximum number of dynamic objects contained sets should allow
     */
    DescriptorSetInstanceCache(std::uint32_t maxStatic, std::uint32_t maxDynamic);

    /**
     * @brief Fetches or creates a descriptor set for the given descriptor set factory
     *
     * @param factory The factory to get or create a set instance for
     * @return The descriptor set instance to use
     */
    DescriptorSetInstance* getDescriptorSet(DescriptorSetFactory* factory);

    /**
     * @brief Updates the camera value for the given observer in each contained scene descriptor set
     *
     * @param observerIndex Index of the observer to update
     * @param projView Camera matrix for the given observer
     */
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);

    /**
     * @brief Goes through all descriptor set instances and calls removeObject for the given object
     *
     * @param sceneId The id of the object within the scene it belongs to
     * @param entity The ECS id of the object to unlink
     */
    void unlinkSceneObject(std::uint32_t sceneId, ecs::Entity entity);

    /**
     * @brief Called once prior to TransferEngine kicking off
     */
    void handleDescriptorSync();

private:
    const std::uint32_t maxStatic;
    const std::uint32_t maxDynamic;
    std::unordered_map<DescriptorSetFactory*, std::unique_ptr<DescriptorSetInstance>> cache;
    std::vector<ds::SceneDescriptorSetInstance*> sceneSets;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
