#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <cstdint>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
namespace ds
{
/**
 * @brief Base class for descriptor sets that exist for each scene. Derived classes are created by
 *        DescriptorSetFactory classes that exist per pipeline config. This allows custom descriptor
 *        sets to be used by scenes
 *
 * @ingroup Renderer
 */
class DescriptorSetInstance {
public:
    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~DescriptorSetInstance() = default;

    /**
     * @brief Returns whether or not this descriptor set needs to be bound per-object
     */
    constexpr bool isPerObject() const;

    /**
     * @brief Called by scene once after the instance is created
     *
     * @param maxStaticObjects The maximum number of static objects in the owning scene
     * @param maxDynamicObjects The maximum number of dynamic objects in the owning scene
     */
    virtual void init(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects) = 0;

    /**
     * @brief Called once after the pipeline is bound. This should bind the descriptor set
     *
     * @param commandBuffer The command buffer to issue bind commands into
     * @param layout Pipeline layout of the currently bound pipeline
     * @param setIndex The index of the descriptor set in the owning scene
     */
    virtual void bindForPipeline(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                 std::uint32_t setIndex) const = 0;

    /**
     * @brief Called per-object by the scene if this instance has per-object logic
     *
     * @param commandBuffer The command buffer to issue bind commands into
     * @param layout Pipeline layout of the currently bound pipeline
     * @param setIndex The index of the descriptor set in the owning scene
     * @param objectId The object id of the current object
     */
    virtual void bindForObject(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                               std::uint32_t setIndex, std::uint32_t objectId) const = 0;

    /**
     * @brief Called by the scene when new objects are created that require this set
     *
     * @param sceneId The 0-based index of this object in the scene
     * @param entity The entity id of this object in the ECS
     * @param updateSpeed Whether the object is expected to be static or dynamic
     * @return True if the object could be added, false otherwise
     */
    virtual bool allocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                SceneObject::UpdateSpeed updateSpeed) = 0;

    /**
     * @brief Called by the scene when an object is destroyed
     *
     * @param sceneId The scene id of the removed object
     * @param entity The ECS id of the object being removed
     */
    virtual void releaseObject(std::uint32_t sceneId, ecs::Entity entity) = 0;

protected:
    /**
     * @brief Creates a new DescriptorSetInstance
     *
     * @param perObject True if the instance has per-object logic, false if only per-pipeline
     */
    DescriptorSetInstance(bool perObject);

private:
    const bool perObject;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr bool DescriptorSetInstance::isPerObject() const { return perObject; }

} // namespace ds
} // namespace render
} // namespace bl

#endif
