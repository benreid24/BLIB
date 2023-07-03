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
     */
    DescriptorSetInstanceCache();

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
     * @param entity The ECS id of the object to unlink
     * @param key The key of the object within the scene it belongs to
     */
    void unlinkSceneObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Called once prior to TransferEngine kicking off
     */
    void handleDescriptorSync();

private:
    std::unordered_map<DescriptorSetFactory*, std::unique_ptr<DescriptorSetInstance>> cache;
    std::vector<ds::SceneDescriptorSetInstance*> sceneSets;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
