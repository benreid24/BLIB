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
     * @param component Pointer to the component being marked dirty
     */
    void markObjectDirty(scene::Key key, void* component);

    /**
     * @brief Flushes underlying buffers for changed components
     */
    virtual void performSync() = 0;

    /**
     * @brief Performs the copy from ECS components to renderer buffers
     */
    virtual void copyFromECS() = 0;

    /**
     * @brief Returns the ranges of dirty dynamic elements
     */
    const DirtyRange& dirtyDynamicRange() const;

    /**
     * @brief Returns the ranges of static dynamic elements
     */
    const DirtyRange& dirtyStaticRange() const;

protected:
    DirtyRange dirtyDynamic;
    DirtyRange dirtyStatic;
    std::vector<void*> dirtyComponents;
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
     * @brief Performs the copy from ECS components to renderer buffers
     */
    virtual void copyFromECS() override;

    /**
     * @brief Returns a reference to the underlying dynamic buffer
     */
    TDynamicStorage& getDynamicBuffer();

    /**
     * @brief Returns a reference to the underlying static buffer
     */
    TStaticStorage& getStaticBuffer();

    /**
     * @brief Returns true if the dynamic descriptor sets need to be updated this frame
     */
    bool dynamicDescriptorUpdateRequired() const;

    /**
     * @brief Returns true if the static descriptor sets need to be updated this frame
     */
    bool staticDescriptorUpdateRequired() const;

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

inline void DescriptorComponentStorageBase::markObjectDirty(scene::Key key, void* component) {
    auto& range = key.updateFreq == UpdateSpeed::Dynamic ? dirtyDynamic : dirtyStatic;
    if (range.start > range.end) {
        range.start = key.sceneId;
        range.end   = key.sceneId;
    }
    else {
        range.start = key.sceneId < range.start ? key.sceneId : range.start;
        range.end   = key.sceneId > range.end ? key.sceneId : range.end;
    }
    dirtyComponents.emplace_back(component);
}

inline const DescriptorComponentStorageBase::DirtyRange&
DescriptorComponentStorageBase::dirtyDynamicRange() const {
    return dirtyDynamic;
}

inline const DescriptorComponentStorageBase::DirtyRange&
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
    dirtyComponents.reserve(Config::DefaultSceneObjectCapacity);
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
bool DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::allocateObject(
    ecs::Entity entity, scene::Key key) {
    TCom* component = registry.getComponent<TCom>(entity);
    if (!component) { return false; }

    if (key.updateFreq == UpdateSpeed::Dynamic) {
        if (dynamicBuffer.ensureSize(key.sceneId + 1)) {
            dynamicRefresh = 0x1 << Config::MaxConcurrentFrames;
        }
        component->link(this, key);

        if (key.sceneId >= dynCounts.size()) { dynCounts.resize(key.sceneId + 1, 0); }
        dynCounts[key.sceneId] += 1;
    }
    else {
        if (staticBuffer.ensureSize(key.sceneId + 1)) {
            statCounts.resize(key.sceneId + 1, 0);
            staticRefresh = 0x1 << Config::MaxConcurrentFrames;
        }
        component->link(this, key);

        if (key.sceneId >= statCounts.size()) { statCounts.resize(key.sceneId + 1, 0); }
        statCounts[key.sceneId] += 1;
    }
    markObjectDirty(key, component);
    return true;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::releaseObject(
    ecs::Entity entity, scene::Key key) {
    auto& refCounts = key.updateFreq == UpdateSpeed::Dynamic ? dynCounts : statCounts;
    if (key.sceneId >= refCounts.size()) { return; }

    auto& refCount = refCounts[key.sceneId];
    if (refCount == 1) {
        TCom* component = registry.getComponent<TCom>(entity);
        if (component) {
            component->unlink();
            std::erase_if(dirtyComponents,
                          [component](void* c) { return static_cast<TCom*>(c) == component; });
        }
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
    else { dynamicBuffer.transferRange(0, 0); }
    dynamicRefresh = dynamicRefresh >> 1;
    staticRefresh  = staticRefresh >> 1;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::copyFromECS() {
    for (void* c : dirtyComponents) {
        TCom* com         = static_cast<TCom*>(c);
        auto& payload     = (com->getSceneKey().updateFreq == UpdateSpeed::Dynamic) ?
                                dynamicBuffer[com->getSceneKey().sceneId] :
                                staticBuffer[com->getSceneKey().sceneId];
        using PayloadType = std::remove_reference_t<decltype(payload)>;
        static_cast<TCom*>(c)->template refresh<PayloadType>(payload);
    }
    dirtyComponents.clear();
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
TDynamicStorage&
DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::getDynamicBuffer() {
    return dynamicBuffer;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
TStaticStorage&
DescriptorComponentStorage<TCom, TPayload, TDynamicStorage, TStaticStorage>::getStaticBuffer() {
    return staticBuffer;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
bool DescriptorComponentStorage<TCom, TPayload, TDynamicStorage,
                                TStaticStorage>::dynamicDescriptorUpdateRequired() const {
    return dynamicRefresh != 0;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
bool DescriptorComponentStorage<TCom, TPayload, TDynamicStorage,
                                TStaticStorage>::staticDescriptorUpdateRequired() const {
    return staticRefresh != 0;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
