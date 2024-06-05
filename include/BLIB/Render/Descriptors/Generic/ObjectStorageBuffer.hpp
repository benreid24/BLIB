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
    ObjectStorageBuffer()
    : Binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
    , components(nullptr) {}

    /**
     * @brief Creates the binding
     */
    virtual ~ObjectStorageBuffer() = default;

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

private:
    DescriptorComponentStorage<TComponent, TPayload, TDynamicStorage, TStaticStorage>* components;
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
    vk::VulkanState&, DescriptorComponentStorageCache& storageCache) {
    components =
        storageCache.getComponentStorage<TComponent, TPayload, TDynamicStorage, TStaticStorage>();
}

template<typename TPayload, typename TComponent, typename TDynamicStorage, typename TStaticStorage>
void ObjectStorageBuffer<TPayload, TComponent, TDynamicStorage, TStaticStorage>::writeSet(
    SetWriteHelper& writer, UpdateSpeed speed, std::uint32_t frameIndex) {
    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = speed == UpdateSpeed::Dynamic ?
                                             components->getDynamicBuffer().getRawBuffer(frameIndex) :
                                             components->getStaticBuffer().getRawBuffer(frameIndex);
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = speed == UpdateSpeed::Dynamic ?
                                             components->getDynamicBuffer().getTotalRange() :
                                             components->getStaticBuffer().getTotalRange();

    VkWriteDescriptorSet& setWrite = writer.getNewSetWrite();
    setWrite.dstBinding            = getBindingIndex();
    setWrite.pBufferInfo           = &bufferInfo;
    setWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
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

} // namespace ds
} // namespace rc
} // namespace bl

#endif
