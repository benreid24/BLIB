#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <cstdint>
#include <glad/vulkan.h>

namespace bl
{
namespace gfx
{
namespace scene
{
class SceneRenderContext;
}

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
    /*
        TODO - Should consider breaking this down into DescriptorInstances that are deduped per
       scene. Reason: Allow same component to be used across different descriptor sets (ie Transform
       if object uses different descriptor set for shadow pass than render pass)
    */

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~DescriptorSetInstance() = default;

    /**
     * @brief Returns whether or not this descriptor set needs to be bound per-object
     */
    constexpr bool isBindless() const;

    /**
     * @brief Called by scene once after the instance is created
     *
     * @param maxStaticObjects The maximum number of static objects in the owning scene
     * @param maxDynamicObjects The maximum number of dynamic objects in the owning scene
     */
    void init(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects);

    /**
     * @brief Called once after the pipeline is bound. This should bind the descriptor set
     *
     * @param ctx Render context containing necessary data
     * @param layout Pipeline layout of the currently bound pipeline
     * @param setIndex The index of the descriptor set in the owning scene
     */
    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex) const = 0;

    /**
     * @brief Called per-object by the scene if this instance has per-object logic
     *
     * @param ctx Render context containing necessary data
     * @param layout Pipeline layout of the currently bound pipeline
     * @param setIndex The index of the descriptor set in the owning scene
     * @param objectId The object id of the current object
     */
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, std::uint32_t objectId) const = 0;

    /**
     * @brief Called by the scene when new objects are created that require this set
     *
     * @param sceneId The 0-based index of this object in the scene
     * @param entity The entity id of this object in the ECS
     * @param updateSpeed Whether the object is expected to be static or dynamic
     * @return True if the object could be added, false otherwise
     */
    bool allocateObject(std::uint32_t sceneId, ecs::Entity entity, UpdateSpeed updateSpeed);

    /**
     * @brief Called by the scene when an object is destroyed
     *
     * @param sceneId The scene id of the removed object
     * @param entity The ECS id of the object being removed
     */
    virtual void releaseObject(std::uint32_t sceneId, ecs::Entity entity) = 0;

    /**
     * @brief Marks the given object's descriptors dirty for this frame
     *
     * @param sceneId The scene id of the object that refreshed descriptors
     */
    void markObjectDirty(std::uint32_t sceneId);

    /**
     * @brief Called once before the TransferEngine starts to send sync commands to buffers
     */
    void handleFrameStart();

protected:
    /**
     * @brief Creates a new DescriptorSetInstance
     *
     * @param bindless True if the descriptor set is bindless for objects, false otherwise
     */
    DescriptorSetInstance(bool bindless);

    /**
     * @brief Called by scene once after the instance is created
     *
     * @param maxStaticObjects The maximum number of static objects in the owning scene
     * @param maxDynamicObjects The maximum number of dynamic objects in the owning scene
     */
    virtual void doInit(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects) = 0;

    /**
     * @brief Called by the scene when new objects are created that require this set
     *
     * @param sceneId The 0-based index of this object in the scene
     * @param entity The entity id of this object in the ECS
     * @param updateSpeed Whether the object is expected to be static or dynamic
     * @return True if the object could be added, false otherwise
     */
    virtual bool doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                  UpdateSpeed updateSpeed) = 0;

    /**
     * @brief Called once each frame before the TransferEngine is kicked off. Use this to
     *        appropriately handle sending descriptor data to the GPU
     *
     * @param staticObjectsChanged True if any static objects were changed, false otherwise
     */
    virtual void beginSync(bool staticObjectsChanged) = 0;

private:
    const bool bindless;
    std::uint32_t maxStatic;
    int staticChanged;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr bool DescriptorSetInstance::isBindless() const { return bindless; }

inline void DescriptorSetInstance::markObjectDirty(std::uint32_t si) {
    staticChanged = si < maxStatic ? Config::MaxConcurrentFrames : staticChanged;
}

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
