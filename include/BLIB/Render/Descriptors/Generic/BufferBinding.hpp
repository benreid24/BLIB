#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_BUFFERBINDING_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_BUFFERBINDING_HPP

#include <BLIB/Render/Descriptors/Generic/Binding.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <BLIB/Render/ShaderResources/ShaderResourceStore.hpp>
#include <BLIB/Render/ShaderResources/StoreKey.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

namespace bl
{
namespace rc
{
namespace ds
{

/**
 * @brief Generic binding provider for a data buffer
 *
 * @tparam TShaderResource The type of shader resource that provides the buffer
 * @tparam ResourceKey The key to get the shader resource with
 * @tparam ResourceStore Which shader resource store to get the buffer from
 * @tparam DescriptorType The type of descriptor to bind the buffer as
 * @ingroup Renderer
 */
template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER>
class BufferBinding : public Binding {
public:
    /**
     * @brief Creates the binding
     */
    BufferBinding()
    : Binding(DescriptorType) {}

    /**
     * @brief Destroys the binding
     */
    virtual ~BufferBinding() = default;

    virtual DescriptorSetInstance::EntityBindMode getBindMode() const override;
    virtual DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override;
    virtual void init(vk::VulkanState& vulkanState, InitContext& ctx) override;
    virtual void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed speed,
                          std::uint32_t frameIndex) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void releaseObject(ecs::Entity entity, scene::Key key) override;
    virtual void onFrameStart() override;
    virtual bool staticDescriptorUpdateRequired() const override;
    virtual bool dynamicDescriptorUpdateRequired() const override;

private:
    TShaderResource* buffer;
    vk::PerFrame<VkBuffer> boundDynamic;
    vk::PerFrame<VkBuffer> boundStatic;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
DescriptorSetInstance::EntityBindMode
BufferBinding<TShaderResource, ResourceKey, ResourceStore, DescriptorType>::getBindMode() const {
    return DescriptorSetInstance::EntityBindMode::Bindless;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
DescriptorSetInstance::SpeedBucketSetting
BufferBinding<TShaderResource, ResourceKey, ResourceStore, DescriptorType>::getSpeedMode() const {
    return DescriptorSetInstance::SpeedBucketSetting::SpeedAgnostic;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
void BufferBinding<TShaderResource, ResourceKey, ResourceStore, DescriptorType>::init(
    vk::VulkanState& vs, InitContext& ctx) {
    buffer = ctx.getShaderResourceStore(ResourceStore).getShaderResourceWithKey(ResourceKey);
    boundDynamic.init(vs, [](VkBuffer& b) { b = nullptr; });
    boundStatic.init(vs, [](VkBuffer& b) { b = nullptr; });
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
void BufferBinding<TShaderResource, ResourceKey, ResourceStore, DescriptorType>::writeSet(
    SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed spd, std::uint32_t frameIndex) {
    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = buffer->getBuffer().getRawBuffer(frameIndex);
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = buffer->getBuffer().getTotalAlignedSize();

    VkWriteDescriptorSet& setWrite = writer.getNewSetWrite(set);
    setWrite.dstBinding            = getBindingIndex();
    setWrite.pBufferInfo           = &bufferInfo;
    setWrite.descriptorType        = DescriptorType;

    auto& bound              = spd == UpdateSpeed::Dynamic ? boundDynamic : boundStatic;
    bound.getRaw(frameIndex) = bufferInfo.buffer;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
bool BufferBinding<TShaderResource, ResourceKey, ResourceStore, DescriptorType>::allocateObject(
    ecs::Entity, scene::Key) {
    // noop, we are global
    return true;
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
void BufferBinding<TShaderResource, ResourceKey, ResourceStore, DescriptorType>::releaseObject(
    ecs::Entity, scene::Key) {
    // noop, we are global
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
void BufferBinding<TShaderResource, ResourceKey, ResourceStore, DescriptorType>::onFrameStart() {
    // noop
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
bool BufferBinding<TShaderResource, ResourceKey, ResourceStore,
                   DescriptorType>::staticDescriptorUpdateRequired() const {
    return buffer->getBuffer().getCurrentFrameRawBuffer() != boundStatic.current();
}

template<typename TShaderResource, sr::Key<TShaderResource> ResourceKey, sr::StoreKey ResourceStore,
         VkDescriptorType DescriptorType>
bool BufferBinding<TShaderResource, ResourceKey, ResourceStore,
                   DescriptorType>::dynamicDescriptorUpdateRequired() const {
    return buffer->getBuffer().getCurrentFrameRawBuffer() != boundDynamic.current();
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
