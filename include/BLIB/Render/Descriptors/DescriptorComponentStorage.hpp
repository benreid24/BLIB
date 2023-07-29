#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORCOMPONENTSTORAGE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORCOMPONENTSTORAGE_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Config.hpp>
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
    /**
     * @brief Represents a range of dirty elements
     */
    struct DirtyRange {
        std::uint32_t start;
        std::uint32_t end;

        DirtyRange()
        : start(1)
        , end(0) {}
    };

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

    /**
     * @brief Returns the ranges of dirty dynamic elements
     */
    constexpr const DirtyRange& dirtyDynamicRange() const;

    /**
     * @brief Returns the ranges of static dynamic elements
     */
    constexpr const DirtyRange& dirtyStaticRange() const;

protected:
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

    /**
     * @brief Returns true if the dynamic descriptor sets need to be updated this frame
     */
    constexpr bool dynamicDescriptorUpdateRequired() const;

    /**
     * @brief Returns true if the static descriptor sets need to be updated this frame
     */
    constexpr bool staticDescriptorUpdateRequired() const;

private:
    ecs::Registry& registry;
    const EntityCallback getEntityFromSceneKey;
    TDynamicStorage dynamicBuffer;
    TStaticStorage staticBuffer;
    std::vector<std::uint8_t> dynCounts;
    std::vector<std::uint8_t> statCounts;
    std::uint8_t dynamicRefresh;
    std::uint8_t staticRefresh;

    void refreshLinks(UpdateSpeed speed);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline void DescriptorComponentStorageBase::markObjectDirty(scene::Key key) {
    auto& range = key.updateFreq == UpdateSpeed::Dynamic ? dirtyDynamic : dirtyStatic;
    range.start = key.sceneId < range.start ? key.sceneId : range.start;
    range.end   = key.sceneId > range.end ? key.sceneId : range.end;
}

inline constexpr const DescriptorComponentStorageBase::DirtyRange&
DescriptorComponentStorageBase::dirtyDynamicRange() const {
    return dirtyDynamic;
}

inline constexpr const DescriptorComponentStorageBase::DirtyRange&
DescriptorComponentStorageBase::dirtyStaticRange() const {
    return dirtyStatic;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::
    DescriptorComponentStorage(ecs::Registry& r, vk::VulkanState& vulkanState,
                               const EntityCallback& entityCb)
: registry(r)
, getEntityFromSceneKey(entityCb)
, dynamicRefresh(0x1 << Config::MaxConcurrentFrames)
, staticRefresh(0x1 << Config::MaxConcurrentFrames) {
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
        if (dynamicBuffer.ensureSize(key.sceneId + 1)) {
            refreshLinks(key.updateFreq);
            dynCounts.resize(key.sceneId + 1, 0);
            dynamicRefresh = 0x1 << Config::MaxConcurrentFrames;
        }
        component->link(this, key, &dynamicBuffer[key.sceneId]);
        dynCounts[key.sceneId] += 1;
    }
    else {
        if (staticBuffer.ensureSize(key.sceneId + 1)) {
            refreshLinks(key.updateFreq);
            statCounts.resize(key.sceneId + 1, 0);
            staticRefresh = 0x1 << Config::MaxConcurrentFrames;
        }
        component->link(this, key, &staticBuffer[key.sceneId]);
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
    dynamicRefresh = dynamicRefresh >> 1;
    staticRefresh  = staticRefresh >> 1;
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

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
constexpr bool DescriptorComponentStorage<TCom, TPayload, TDynamicStorage,
                                          TStaticStorage>::dynamicDescriptorUpdateRequired() const {
    return dynamicRefresh != 0;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
constexpr bool DescriptorComponentStorage<TCom, TPayload, TDynamicStorage,
                                          TStaticStorage>::staticDescriptorUpdateRequired() const {
    return staticRefresh != 0;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
