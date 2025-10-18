#ifndef BLIB_RENDER_DESCRIPTORS_OBJECTSTORAGEBUFFER_HPP
#define BLIB_RENDER_DESCRIPTORS_OBJECTSTORAGEBUFFER_HPP

#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>
#include <BLIB/Render/ShaderResources/EntityComponentShaderResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>

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
template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, bool Optional = false>
class ObjectBufferBinding : public Binding {
public:
    /**
     * @brief Creates the binding
     */
    ObjectBufferBinding()
    : Binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
    , components(nullptr) {}

    /**
     * @brief Creates the binding
     */
    virtual ~ObjectBufferBinding() = default;

    DescriptorSetInstance::EntityBindMode getBindMode() const override;
    DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override;
    void init(vk::VulkanState& vulkanState, sr::ShaderResourceStore& globalShaderResources,
              sr::ShaderResourceStore& sceneShaderResources,
              sr::ShaderResourceStore& observerShaderResources) override;
    void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed speed,
                  std::uint32_t frameIndex) override;
    bool allocateObject(ecs::Entity entity, scene::Key key) override;
    void releaseObject(ecs::Entity entity, scene::Key key) override;
    void onFrameStart() override;
    bool staticDescriptorUpdateRequired() const override;
    bool dynamicDescriptorUpdateRequired() const override;

private:
    TShaderResource* components;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
DescriptorSetInstance::EntityBindMode
ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType, Optional>::getBindMode() const {
    return DescriptorSetInstance::EntityBindMode::Bindless;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
DescriptorSetInstance::SpeedBucketSetting
ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType, Optional>::getSpeedMode() const {
    return DescriptorSetInstance::SpeedBucketSetting::RebindForNewSpeed;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
void ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType, Optional>::init(
    vk::VulkanState&, sr::ShaderResourceStore&, sr::ShaderResourceStore& sceneShaderResources,
    sr::ShaderResourceStore&) {
    components = sceneShaderResources.getShaderResourceWithKey(ResourceKey);
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
void ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType, Optional>::writeSet(
    SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed speed, std::uint32_t frameIndex) {
    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = speed == UpdateSpeed::Dynamic ?
                                             components->getDynamicBuffer().getRawBuffer(frameIndex) :
                                             components->getStaticBuffer().getRawBuffer(frameIndex);
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = speed == UpdateSpeed::Dynamic ?
                                             components->getDynamicBuffer().getTotalRange() :
                                             components->getStaticBuffer().getTotalRange();

    VkWriteDescriptorSet& setWrite = writer.getNewSetWrite(set);
    setWrite.dstBinding            = getBindingIndex();
    setWrite.pBufferInfo           = &bufferInfo;
    setWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
bool ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType, Optional>::allocateObject(
    ecs::Entity entity, scene::Key key) {
    return components->allocateObject(entity, key) || Optional;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
void ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType, Optional>::releaseObject(
    ecs::Entity entity, scene::Key key) {
    components->releaseObject(entity, key);
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
void ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType, Optional>::onFrameStart() {
    // noop
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
bool ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType,
                         Optional>::staticDescriptorUpdateRequired() const {
    return components->staticDescriptorUpdateRequired();
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey,
         VkDescriptorType DescriptorType, bool Optional>
bool ObjectBufferBinding<TShaderResource, ResourceKey, DescriptorType,
                         Optional>::dynamicDescriptorUpdateRequired() const {
    return components->dynamicDescriptorUpdateRequired();
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
