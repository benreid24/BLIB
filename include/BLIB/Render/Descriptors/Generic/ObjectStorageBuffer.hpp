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
 * @tparam T The shader format of the data to provide
 * @tparam TComponent The ECS component to populate from
 * @tparam Optional Whether or not this binding is optional for objects
 * @tparam TDynamicStorage The type of buffer to use for dynamic entities
 * @tparam TStaticStorage The type of buffer to use for static entities
 * @ingroup Renderer
 */
template<typename T, typename TComponent = T, bool Optional = false,
         typename TDynamicStorage = buf::DynamicSSBO<T>,
         typename TStaticStorage  = buf::StaticSSBO<T>>
class ObjectStorageBuffer : public Binding {
public:
    using TPayload = T;

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
    DescriptorComponentStorage<TComponent, T, TDynamicStorage, TStaticStorage>* components;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
DescriptorSetInstance::BindMode
ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage, TStaticStorage>::getBindMode() const {
    return DescriptorSetInstance::BindMode::Bindless;
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
DescriptorSetInstance::SpeedBucketSetting ObjectStorageBuffer<
    T, TComponent, Optional, TDynamicStorage, TStaticStorage>::getSpeedMode() const {
    return DescriptorSetInstance::SpeedBucketSetting::RebindForNewSpeed;
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
void ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage, TStaticStorage>::init(
    vk::VulkanState&, DescriptorComponentStorageCache& storageCache) {
    components = storageCache.getComponentStorage<TComponent, T, TDynamicStorage, TStaticStorage>();
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
void ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage, TStaticStorage>::writeSet(
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

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
bool ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage, TStaticStorage>::allocateObject(
    ecs::Entity entity, scene::Key key) {
    return components->allocateObject(entity, key) || Optional;
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
void ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage, TStaticStorage>::releaseObject(
    ecs::Entity entity, scene::Key key) {
    components->releaseObject(entity, key);
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
void ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage, TStaticStorage>::onFrameStart() {
    // noop
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
void* ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage, TStaticStorage>::getPayload() {
    // no payload
    return nullptr;
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
bool ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage,
                         TStaticStorage>::staticDescriptorUpdateRequired() const {
    return components->staticDescriptorUpdateRequired();
}

template<typename T, typename TComponent, bool Optional, typename TDynamicStorage,
         typename TStaticStorage>
bool ObjectStorageBuffer<T, TComponent, Optional, TDynamicStorage,
                         TStaticStorage>::dynamicDescriptorUpdateRequired() const {
    return components->dynamicDescriptorUpdateRequired();
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
