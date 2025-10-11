#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_GLOBALUNIFORMBUFFER_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_GLOBALUNIFORMBUFFER_HPP

#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Generic binding provider for a global uniform buffer
 *
 * @tparam T The payload type to provide globally to shaders
 * @ingroup Renderer
 */
template<typename T>
class GlobalUniformBuffer : public Binding {
public:
    using TPayload = T;

    /**
     * @brief Creates the binding
     */
    GlobalUniformBuffer()
    : Binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {}

    /**
     * @brief Destroys the binding
     */
    virtual ~GlobalUniformBuffer() = default;

    DescriptorSetInstance::EntityBindMode getBindMode() const override;
    DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override;
    void init(vk::VulkanState& vulkanState, sr::ShaderResourceStore& storageCache) override;
    void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed speed,
                  std::uint32_t frameIndex) override;
    bool allocateObject(ecs::Entity entity, scene::Key key) override;
    void releaseObject(ecs::Entity entity, scene::Key key) override;
    void onFrameStart() override;
    void* getPayload() override;
    bool staticDescriptorUpdateRequired() const override;
    bool dynamicDescriptorUpdateRequired() const override;

private:
    T value;
    buf::UniformBuffer<T> buffer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
DescriptorSetInstance::EntityBindMode GlobalUniformBuffer<T>::getBindMode() const {
    return DescriptorSetInstance::EntityBindMode::Bindless;
}

template<typename T>
DescriptorSetInstance::SpeedBucketSetting GlobalUniformBuffer<T>::getSpeedMode() const {
    return DescriptorSetInstance::SpeedBucketSetting::SpeedAgnostic;
}

template<typename T>
void GlobalUniformBuffer<T>::init(vk::VulkanState& vs, sr::ShaderResourceStore&) {
    buffer.create(vs, 1);
    buffer.transferEveryFrame();
}

template<typename T>
void GlobalUniformBuffer<T>::writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed,
                                      std::uint32_t frameIndex) {
    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = buffer.gpuBufferHandles().getRaw(frameIndex).getBuffer();
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = buffer.alignedUniformSize();

    VkWriteDescriptorSet& setWrite = writer.getNewSetWrite(set);
    setWrite.dstBinding            = getBindingIndex();
    setWrite.pBufferInfo           = &bufferInfo;
    setWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

template<typename T>
bool GlobalUniformBuffer<T>::allocateObject(ecs::Entity, scene::Key) {
    // noop, we are global
    return true;
}

template<typename T>
void GlobalUniformBuffer<T>::releaseObject(ecs::Entity, scene::Key) {
    // noop, we are global
}

template<typename T>
void GlobalUniformBuffer<T>::onFrameStart() {
    buffer[0] = value;
}

template<typename T>
void* GlobalUniformBuffer<T>::getPayload() {
    return &value;
}

template<typename T>
bool GlobalUniformBuffer<T>::staticDescriptorUpdateRequired() const {
    return false;
}

template<typename T>
bool GlobalUniformBuffer<T>::dynamicDescriptorUpdateRequired() const {
    return false;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
