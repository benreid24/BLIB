#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETINSTANCE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
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
    /// Bind mode of the descriptor set
    enum BindMode { Bindless, Bindful };

    /// Whether or not the descriptor set needs to be re-bound for different update speeds
    enum SpeedBucketSetting { RebindForNewSpeed, SpeedAgnostic };

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~DescriptorSetInstance() = default;

    /**
     * @brief Returns whether or not this descriptor set needs to be bound per-object
     */
    constexpr bool isBindless() const;

    /**
     * @brief Returns whether or not the descriptor set needs to be re-bound for new speed
     */
    constexpr bool needsRebindForNewSpeed() const;

    /**
     * @brief Called by scene once after the instance is created
     */
    virtual void init() = 0;

    /**
     * @brief Called once after the pipeline is bound. This should bind the descriptor set
     *
     * @param ctx Render context containing necessary data
     * @param layout Pipeline layout of the currently bound pipeline
     * @param setIndex The index of the descriptor set in the owning scene
     * @param updateFreq The update frequency of the descriptors to bind
     */
    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const = 0;

    /**
     * @brief Called per-object by the scene if this instance has per-object logic
     *
     * @param ctx Render context containing necessary data
     * @param layout Pipeline layout of the currently bound pipeline
     * @param setIndex The index of the descriptor set in the owning scene
     * @param objectKey The key of the current object
     */
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const = 0;

    /**
     * @brief Called by the scene when new objects are created that require this set
     *
     * @param entity The entity id of this object in the ECS
     * @param key The scene key of the new object
     * @return True if the object could be added, false otherwise
     */
    bool allocateObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Called by the scene when an object is destroyed
     *
     * @param entity The ECS id of the object being removed
     * @param key The scene key of the object being removed
     */
    virtual void releaseObject(ecs::Entity entity, scene::Key key) = 0;

    /**
     * @brief Marks the given object's descriptors dirty for this frame
     *
     * @param key The scene key of the object that refreshed descriptors
     */
    void markObjectDirty(scene::Key key);

    /**
     * @brief Called once before the TransferEngine starts to send sync commands to buffers
     */
    void handleFrameStart();

protected:
    struct DirtyRange {
        std::uint32_t start;
        std::uint32_t size;
    };

    /**
     * @brief Creates a new DescriptorSetInstance
     *
     * @param bindMode The bind mode of the descriptor set
     * @param speedSetting The speed re-bind setting of the descriptor set
     */
    DescriptorSetInstance(BindMode bindMode, SpeedBucketSetting speedSetting);

    /**
     * @brief Called by the scene when new objects are created that require this set
     *
     * @param entity The entity id of this object in the ECS
     * @param key The scene key of the new object
     * @return True if the object could be added, false otherwise
     */
    virtual bool doAllocateObject(ecs::Entity entity, scene::Key key) = 0;

    /**
     * @brief Called once each frame before the TransferEngine is kicked off. Use this to
     *        appropriately handle sending descriptor data to the GPU
     *
     * @param dirtyStatic The range of static objects that need to be synced
     * @param dirtyDynamic The range of dynamic objects that need to be synced
     */
    virtual void beginSync(DirtyRange dirtyStatic, DirtyRange dirtyDynamic) = 0;

private:
    const bool bindless;
    const bool speedBind;
    DirtyRange dirtyStatic;
    DirtyRange dirtyDynamic;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr bool DescriptorSetInstance::isBindless() const { return bindless; }

inline constexpr bool DescriptorSetInstance::needsRebindForNewSpeed() const { return speedBind; }

inline void DescriptorSetInstance::markObjectDirty(scene::Key key) {
    auto& range = key.updateFreq == UpdateSpeed::Static ? dirtyStatic : dirtyDynamic;
    range.start = key.sceneId < range.start ? key.sceneId : range.start;
    range.size  = key.sceneId > range.size ? key.sceneId : range.size; // use as end here
}

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
