#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_GLOBALSTORAGEBUFFER_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_GLOBALSTORAGEBUFFER_HPP

#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Generic descriptor binding for a global SSBO for shaders
 *
 * @tparam T The data to provide to shaders globally
 * @tparam TStorage The buffer type to use
 * @ingroup Renderer
 */
template<typename T, typename TStorage = buf::StaticSSBO<T>>
class GlobalStorageBuffer : public Binding {
public:
    /**
     * @brief Creates the binding
     */
    GlobalStorageBuffer()
    : Binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {}

    /**
     * @brief Destroys the binding
     */
    virtual ~GlobalStorageBuffer() = default;

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
    TStorage storage;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename TStorage>
DescriptorSetInstance::EntityBindMode GlobalStorageBuffer<T, TStorage>::getBindMode() const {
    return DescriptorSetInstance::EntityBindMode::Bindless;
}

template<typename T, typename TStorage>
DescriptorSetInstance::SpeedBucketSetting GlobalStorageBuffer<T, TStorage>::getSpeedMode() const {
    return DescriptorSetInstance::SpeedBucketSetting::SpeedAgnostic;
}

template<typename T, typename TStorage>
void GlobalStorageBuffer<T, TStorage>::init(vk::VulkanState& vulkanState,
                                            sr::ShaderResourceStore& globalShaderResources,
                                            sr::ShaderResourceStore& sceneShaderResources,
                                            sr::ShaderResourceStore& observerShaderResources) {
    // TODO - get buffer from resource store
    // TODO - parameterize which resource store to use
    storage.create(vulkanState, 1);
    storage.transferAll();
    storage.transferEveryFrame();
}

template<typename T, typename TStorage>
void GlobalStorageBuffer<T, TStorage>::writeSet(SetWriteHelper& writer, VkDescriptorSet set,
                                                UpdateSpeed speed, std::uint32_t frameIndex) {
    VkBuffer buf;
    if constexpr (std::is_same_v<TStorage, buf::StaticSSBO<T>>) {
        buf = storage.gpuBufferHandle().getBuffer();
    }
    else { buf = storage.gpuBufferHandles().getRaw(frameIndex).getBuffer(); }

    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = buf;
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = storage.getTotalRange();

    VkWriteDescriptorSet& setWrite = writer.getNewSetWrite(set);
    setWrite.dstBinding            = getBindingIndex();
    setWrite.pBufferInfo           = &bufferInfo;
    setWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
}

template<typename T, typename TStorage>
bool GlobalStorageBuffer<T, TStorage>::allocateObject(ecs::Entity, scene::Key) {
    // noop, we are global
    return true;
}

template<typename T, typename TStorage>
void GlobalStorageBuffer<T, TStorage>::releaseObject(ecs::Entity entity, scene::Key key) {
    // noop, we are global
}

template<typename T, typename TStorage>
void GlobalStorageBuffer<T, TStorage>::onFrameStart() {}

template<typename T, typename TStorage>
bool GlobalStorageBuffer<T, TStorage>::staticDescriptorUpdateRequired() const {
    return false;
}

template<typename T, typename TStorage>
bool GlobalStorageBuffer<T, TStorage>::dynamicDescriptorUpdateRequired() const {
    return false;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
