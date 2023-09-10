#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORCOMPONENTSTORAGE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORCOMPONENTSTORAGE_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>
#include <functional>
#include <vector>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Base class for descriptor component storage modules
 *
 * @ingroup Renderer
 */
class DescriptorComponentStorageBase {
public:
    /// Callback signature to fetch entities from scene ids
    using EntityCallback = std::function<ecs::Entity(scene::Key)>;

    /**
     * @brief Does nothing
     */
    virtual ~DescriptorComponentStorageBase() = default;

    /**
     * @brief Marks the given object's descriptors dirty for this frame
     *
     * @param key The scene key of the object that refreshed descriptors
     */
    void markObjectDirty(scene::Key key);

    /**
     * @brief Flushes underlying buffers for changed components
     */
    virtual void performSync() = 0;

protected:
    struct DirtyRange {
        std::uint32_t start;
        std::uint32_t end;

        DirtyRange()
        : start(1)
        , end(0) {}
    };

    DirtyRange dirtyDynamic;
    DirtyRange dirtyStatic;
};

/**
 * @brief Generic class to provide per-object storage for descriptor set components
 *
 * @tparam TCom The component type in the ECS
 * @tparam TPayload The data type in the descriptor set
 * @tparam TDynamicStorage The buffer to use for dynamic objects
 * @tparam TStaticStorage The buffer to use for static objects
 * @ingroup Renderer
 */
template<typename TCom, typename TPayload, typename TDynamicStorage = buf::DynamicSSBO<TPayload>,
         typename TStaticStorage = buf::StaticSSBO<TPayload>>
class DescriptorComponentStorage : public DescriptorComponentStorageBase {
public:
    /**
     * @brief Creates a new storage module
     *
     * @param registry The ECS registry to fetch components from
     * @param vulkanState The Renderer Vulkan state
     * @param entityCb The callback to map scene keys to ECS keys
     */
    DescriptorComponentStorage(ecs::Registry& registry, vk::VulkanState& vulkanState,
                               const EntityCallback& entityCb);

    /**
     * @brief Frees resources
     */
    virtual ~DescriptorComponentStorage() = default;

    /**
     * @brief Ensures storage capacity and links the component to this buffer
     *
     * @param entity The entity to link
     * @param key The scene id of the new entity
     * @return True if the component was able to be linked, false otherwise
     */
    bool allocateObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Unlinks the component from this buffer
     *
     * @param entity The entity to unlink
     * @param key The scene key
     */
    void releaseObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Flushes underlying buffers for changed components
     */
    virtual void performSync() override;

    /**
     * @brief Returns a reference to the underlying dynamic buffer
     */
    constexpr TDynamicStorage& getDynamicBuffer();

    /**
     * @brief Returns a reference to the underlying static buffer
     */
    constexpr TStaticStorage& getStaticBuffer();

private:
    ecs::Registry& registry;
    const EntityCallback getEntityFromSceneKey;
    TDynamicStorage dynamicBuffer;
    TStaticStorage staticBuffer;
    std::vector<std::uint8_t> dynCounts;
    std::vector<std::uint8_t> statCounts;

    void refreshLinks(UpdateSpeed speed);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline void DescriptorComponentStorageBase::markObjectDirty(scene::Key key) {
    auto& range = key.updateFreq == UpdateSpeed::Dynamic ? dirtyDynamic : dirtyStatic;
    range.start = key.sceneId < range.start ? key.sceneId : range.start;
    range.end   = key.sceneId > range.end ? key.sceneId : range.end;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::
    DescriptorComponentStorage(ecs::Registry& r, vk::VulkanState& vulkanState,
                               const EntityCallback& entityCb)
: registry(r)
, getEntityFromSceneKey(entityCb) {
    dynamicBuffer.create(vulkanState, Config::DefaultSceneObjectCapacity);
    staticBuffer.create(vulkanState, Config::DefaultSceneObjectCapacity);
    dynCounts.resize(Config::DefaultSceneObjectCapacity, 0);
    statCounts.resize(Config::DefaultSceneObjectCapacity, 0);
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
bool DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::allocateObject(
    ecs::Entity entity, scene::Key key) {
    TCom* component = registry.getComponent<TCom>(entity);
    if (!component) { return false; }

    if (key.updateFreq == UpdateSpeed::Dynamic) {
        if (dynamicBuffer.ensureSize(key.sceneId + 1)) { refreshLinks(key.updateFreq); }
        component->link(this, key, &dynamicBuffer[key.sceneId]);
        dynCounts.resize(key.sceneId + 1, 0);
        dynCounts[key.sceneId] += 1;
    }
    else {
        if (staticBuffer.ensureSize(key.sceneId + 1)) { refreshLinks(key.updateFreq); }
        component->link(this, key, &staticBuffer[key.sceneId]);
        statCounts.resize(key.sceneId + 1, 0);
        statCounts[key.sceneId] += 1;
    }
    markObjectDirty(key);
    return true;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::releaseObject(
    ecs::Entity entity, scene::Key key) {
    auto& refCount =
        key.updateFreq == UpdateSpeed::Dynamic ? dynCounts[key.sceneId] : statCounts[key.sceneId];
    if (refCount == 1) {
        TCom* component = registry.getComponent<TCom>(entity);
        if (component) { component->unlink(); }
    }
    refCount = refCount > 0 ? refCount - 1 : 0;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::performSync() {
    if (dirtyStatic.end >= dirtyStatic.start) {
        staticBuffer.transferRange(dirtyStatic.start, dirtyStatic.end - dirtyStatic.start + 1);
        dirtyStatic = DirtyRange();
    }
    if (dirtyDynamic.end >= dirtyDynamic.start) {
        dynamicBuffer.transferRange(dirtyDynamic.start, dirtyDynamic.end - dirtyDynamic.start + 1);
        dirtyDynamic = DirtyRange();
    }
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
constexpr TDynamicStorage&
DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::getDynamicBuffer() {
    return dynamicBuffer;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
constexpr TStaticStorage&
DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::getStaticBuffer() {
    return staticBuffer;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::refreshLinks(
    UpdateSpeed speed) {
    scene::Key key(speed, 0);
    const std::uint32_t end =
        speed == UpdateSpeed::Dynamic ? dynamicBuffer.size() : staticBuffer.size();
    TPayload* payload = speed == UpdateSpeed::Dynamic ? &dynamicBuffer[0] : &staticBuffer[0];
    for (; key.sceneId < end; ++key.sceneId, ++payload) {
        const ecs::Entity entity = getEntityFromSceneKey(key);
        if (entity != ecs::InvalidEntity) {
            TCom* component = registry.getComponent<TCom>(entity);
            if (component) { component->link(this, {speed, scene::Key::InvalidSceneId}, payload); }
        }
    }
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
