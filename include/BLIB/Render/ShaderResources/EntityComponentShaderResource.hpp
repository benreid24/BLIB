#ifndef BLIB_RENDER_DESCRIPTORS_ENTITYCOMPONENTSHADERINPUT_HPP
#define BLIB_RENDER_DESCRIPTORS_ENTITYCOMPONENTSHADERINPUT_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Engine/HeaderHelpers.hpp>
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Config/Constants.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>
#include <functional>
#include <vector>

namespace bl
{
namespace rc
{
namespace sr
{
/**
 * @brief Base class for shader inputs derived from ECS components
 *
 * @ingroup Renderer
 */
class EntityComponentShaderResourceBase : public ShaderResource {
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

    /**
     * @brief Does nothing
     */
    virtual ~EntityComponentShaderResourceBase() = default;

    /**
     * @brief Marks the given object's descriptors dirty for this frame
     *
     * @param key The scene key of the object that refreshed descriptors
     * @param component Pointer to the component being marked dirty
     */
    void markObjectDirty(scene::Key key, void* component);

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
class EntityComponentShaderResource : public EntityComponentShaderResourceBase {
public:
    /**
     * @brief Creates a new component backed shader input
     */
    EntityComponentShaderResource();

    /**
     * @brief Frees resources
     */
    virtual ~EntityComponentShaderResource() = default;

    /**
     * @brief Creates storage buffers
     *
     * @param engine The game engine instance
     * @param vulkanState The renderer Vulkan state
     * @param Unused
     * @param Unused
     */
    virtual void init(engine::Engine& engine, vk::VulkanState& vulkanState, Scene&,
                      const scene::MapKeyToEntityCb& entityMapCb) override;

    /**
     * @brief Releases resources
     */
    virtual void cleanup() override;

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
    virtual void performGpuSync() override;

    /**
     * @brief Performs the copy from ECS components to renderer buffers
     */
    virtual void copyFromSource() override;

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
    virtual bool dynamicDescriptorUpdateRequired() const override;

    /**
     * @brief Returns true if the static descriptor sets need to be updated this frame
     */
    virtual bool staticDescriptorUpdateRequired() const override;

private:
    ecs::Registry* registry;
    scene::MapKeyToEntityCb getEntityFromSceneKey;
    TDynamicStorage dynamicBuffer;
    TStaticStorage staticBuffer;
    std::vector<std::uint8_t> dynCounts;
    std::vector<std::uint8_t> statCounts;
    std::uint8_t dynamicRefresh;
    std::uint8_t staticRefresh;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline void EntityComponentShaderResourceBase::markObjectDirty(scene::Key key, void* component) {
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

inline const EntityComponentShaderResourceBase::DirtyRange&
EntityComponentShaderResourceBase::dirtyDynamicRange() const {
    return dirtyDynamic;
}

inline const EntityComponentShaderResourceBase::DirtyRange&
EntityComponentShaderResourceBase::dirtyStaticRange() const {
    return dirtyStatic;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
EntityComponentShaderResource<TCom, TPayload, TDynamicStorage,
                              TStaticStorage>::EntityComponentShaderResource()
: dynamicRefresh(0x1 << cfg::Limits::MaxConcurrentFrames)
, staticRefresh(0x1 << cfg::Limits::MaxConcurrentFrames) {}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void EntityComponentShaderResource<TCom, TPayload, TDynamicStorage, TStaticStorage>::init(
    engine::Engine& engine, vk::VulkanState& vulkanState, Scene&,
    const scene::MapKeyToEntityCb& entityCb) {
    registry              = &engine::HeaderHelpers::getRegistry(engine);
    getEntityFromSceneKey = entityCb;
    dynamicBuffer.create(vulkanState, cfg::Constants::DefaultSceneObjectCapacity);
    staticBuffer.create(vulkanState, cfg::Constants::DefaultSceneObjectCapacity);
    dynCounts.resize(cfg::Constants::DefaultSceneObjectCapacity, 0);
    statCounts.resize(cfg::Constants::DefaultSceneObjectCapacity, 0);
    dirtyComponents.reserve(cfg::Constants::DefaultSceneObjectCapacity);
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void EntityComponentShaderResource<TCom, TPayload, TDynamicStorage, TStaticStorage>::cleanup() {
    // handled by destructors
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
bool EntityComponentShaderResource<TCom, TPayload, TDynamicStorage, TStaticStorage>::allocateObject(
    ecs::Entity entity, scene::Key key) {
    TCom* component = registry->getComponent<TCom>(entity);
    if (!component) { return false; }

    if (key.updateFreq == UpdateSpeed::Dynamic) {
        if (dynamicBuffer.ensureSize(key.sceneId + 1)) {
            dynamicRefresh = 0x1 << cfg::Limits::MaxConcurrentFrames;
        }
        component->link(this, key);

        if (key.sceneId >= dynCounts.size()) { dynCounts.resize(key.sceneId + 1, 0); }
        dynCounts[key.sceneId] += 1;
    }
    else {
        if (staticBuffer.ensureSize(key.sceneId + 1)) {
            statCounts.resize(key.sceneId + 1, 0);
            staticRefresh = 0x1 << cfg::Limits::MaxConcurrentFrames;
        }
        component->link(this, key);

        if (key.sceneId >= statCounts.size()) { statCounts.resize(key.sceneId + 1, 0); }
        statCounts[key.sceneId] += 1;
    }
    markObjectDirty(key, component);
    return true;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void EntityComponentShaderResource<TCom, TPayload, TDynamicStorage, TStaticStorage>::releaseObject(
    ecs::Entity entity, scene::Key key) {
    auto& refCounts = key.updateFreq == UpdateSpeed::Dynamic ? dynCounts : statCounts;
    if (key.sceneId >= refCounts.size()) { return; }

    auto& refCount = refCounts[key.sceneId];
    if (refCount == 1) {
        TCom* component = registry->getComponent<TCom>(entity);
        if (component) {
            component->unlink();
            std::erase_if(dirtyComponents,
                          [component](void* c) { return static_cast<TCom*>(c) == component; });
        }
    }
    refCount = refCount > 0 ? refCount - 1 : 0;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
void EntityComponentShaderResource<TCom, TPayload, TDynamicStorage,
                                   TStaticStorage>::performGpuSync() {
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
void EntityComponentShaderResource<TCom, TPayload, TDynamicStorage,
                                   TStaticStorage>::copyFromSource() {
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
EntityComponentShaderResource<TCom, TPayload, TDynamicStorage, TStaticStorage>::getDynamicBuffer() {
    return dynamicBuffer;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
TStaticStorage&
EntityComponentShaderResource<TCom, TPayload, TDynamicStorage, TStaticStorage>::getStaticBuffer() {
    return staticBuffer;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
bool EntityComponentShaderResource<TCom, TPayload, TDynamicStorage,
                                   TStaticStorage>::dynamicDescriptorUpdateRequired() const {
    return dynamicRefresh != 0;
}

template<typename TCom, typename TPayload, typename TDynamicStorage, typename TStaticStorage>
bool EntityComponentShaderResource<TCom, TPayload, TDynamicStorage,
                                   TStaticStorage>::staticDescriptorUpdateRequired() const {
    return staticRefresh != 0;
}

} // namespace sr
} // namespace rc
} // namespace bl

#endif
