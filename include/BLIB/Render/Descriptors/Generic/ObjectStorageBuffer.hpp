#ifndef BLIB_RENDER_DESCRIPTORS_OBJECTSTORAGEBUFFER_HPP
#define BLIB_RENDER_DESCRIPTORS_OBJECTSTORAGEBUFFER_HPP

#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Descriptors/DescriptorComponentStorage.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Generic descriptor set binding providing ECS entity components to shaders
 *
 * @tparam TPayload The shader format of the data to provide
 * @tparam TComponent The ECS component to populate from
 * @tparam TDynamicStorage The type of buffer to use for dynamic entities
 * @tparam TStaticStorage The type of buffer to use for static entities
 * @ingroup Renderer
 */
template<typename TPayload, typename TComponent = TPayload,
         typename TDynamicStorage = buf::DynamicSSBO<TPayload>,
         typename TStaticStorage  = buf::StaticSSBO<TPayload>>
class ObjectStorageBuffer : public Binding {
public:
    /**
     * @brief Creates the binding
     */
    ObjectStorageBuffer() = default;

    /**
     * @brief Creates the binding
     */
    virtual ~ObjectStorageBuffer() = default;

private:
    DescriptorComponentStorage<TComponent, TPayload, TDynamicStorage, TStaticStorage>* components;

    DescriptorSetInstance::BindMode getBindMode() const override;
    DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override;
    void init(vk::VulkanState& vulkanState, DescriptorComponentStorageCache& storageCache) override;
    void writeSet(SetWriteHelper& writer, UpdateSpeed speed, std::uint32_t frameIndex) override;
    bool allocateObject(ecs::Entity entity, scene::Key key) override;
    void releaseObject(ecs::Entity entity, scene::Key key) override;
    void onFrameStart() override;
    void* getPayload() override;
    bool staticDescriptorUpdateRequired() const override;
    bool dynamicDescriptorUpdateRequired() const override;

    template<typename TStorage>
    void writeSetHelper(SetWriteHelper& writer, std::uint32_t frameIndex, TStorage& storage);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
DescriptorSetInstance::BindMode
ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::getBindMode() const {
    return DescriptorSetInstance::BindMode::Bindless;
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
DescriptorSetInstance::SpeedBucketSetting
ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::getSpeedMode() const {
    return DescriptorSetInstance::SpeedBucketSetting::RebindForNewSpeed;
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
void ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::init(
    vk::VulkanState& vulkanState, DescriptorComponentStorageCache& storageCache) {
    components =
        storageCache.getComponentStorage<TComponent, TPayload, TDynamicStorage, TStaticStorage>();
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
void ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::writeSet(
    SetWriteHelper& writer, UpdateSpeed speed, std::uint32_t frameIndex) {
    if (speed == UpdateSpeed::Dynamic) {
        writeSetHelper<TDynamicStorage>(writer, frameIndex, components->getDynamicBuffer());
    }
    else { writeSetHelper<TStaticStorage>(writer, frameIndex, components->getStaticBuffer()); }
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
bool ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::allocateObject(
    ecs::Entity entity, scene::Key key) {
    return components->allocateObject(entity, key);
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
void ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::releaseObject(
    ecs::Entity entity, scene::Key key) {
    components->releaseObject(entity, key);
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
void ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::onFrameStart() {
    // noop
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
void* ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::getPayload() {
    // no payload
    return nullptr;
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
bool ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage,
                         TStaticStorage>::staticDescriptorUpdateRequired() const {
    return components->staticDescriptorUpdateRequired();
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
bool ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage,
                         TStaticStorage>::dynamicDescriptorUpdateRequired() const {
    return components->dynamicDescriptorUpdateRequired();
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
template<typename TStorage>
void ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::writeSetHelper(
    SetWriteHelper& writer, std::uint32_t frameIndex, TStorage& storage) {
    VkBuffer buf;
    if constexpr (std::is_same_v<TStorage, buf::StaticSSBO<TPayload>>) {
        buf = storage.gpuBufferHandle().getBuffer();
    }
    else { buf = storage.gpuBufferHandles().getRaw(frameIndex).getBuffer(); }

    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = buf;
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = storage.getTotalRange();

    VkWriteDescriptorSet& setWrite = writer.getNewSetWrite();
    setWrite.dstBinding            = getBindingIndex();
    setWrite.pBufferInfo           = &bufferInfo;
    setWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
