#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
SceneDescriptorSetInstance::SceneDescriptorSetInstance(vk::VulkanState& vulkanState,
                                                       VkDescriptorSetLayout layout)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, vulkanState(vulkanState)
, setLayout(layout) {}

void SceneDescriptorSetInstance::cleanup() {
    vulkanState.descriptorPool.release(allocHandle, descriptorSets.data());
    cameraBuffer.destroy();
}

void SceneDescriptorSetInstance::updateObserverCamera(std::uint32_t observerIndex,
                                                      const ObserverInfo& cam) {
    cameraBuffer[observerIndex] = cam;
}

void SceneDescriptorSetInstance::createCameraBuffer() {
    cameraBuffer.create(vulkanState, Config::MaxSceneObservers);
    cameraBuffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
}

void SceneDescriptorSetInstance::writeCameraDescriptor(SetWriteHelper& writer,
                                                       std::uint32_t observerIndex,
                                                       std::uint32_t frameIndex,
                                                       std::uint32_t binding) {
    VkDescriptorBufferInfo& cameraBufferInfo = writer.getNewBufferInfo();
    cameraBufferInfo.buffer = cameraBuffer.gpuBufferHandles().getRaw(frameIndex).getBuffer();
    cameraBufferInfo.offset = 0;
    cameraBufferInfo.range  = cameraBuffer.alignedUniformSize();

    VkWriteDescriptorSet& cameraWrite =
        writer.getNewSetWrite(descriptorSets.getRaw(frameIndex, observerIndex));
    cameraWrite.descriptorCount = 1;
    cameraWrite.dstBinding      = binding;
    cameraWrite.dstArrayElement = 0;
    cameraWrite.pBufferInfo     = &cameraBufferInfo;
    cameraWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

std::uint32_t SceneDescriptorSetInstance::getBindDynamicOffset(std::uint32_t i) const {
    return i * cameraBuffer.alignedUniformSize();
}

VkDescriptorSetLayoutBinding SceneDescriptorSetInstance::getCameraBufferBindingInfo(
    std::uint32_t binding, VkShaderStageFlags shaderStages) {
    VkDescriptorSetLayoutBinding bindingInfo{};

    bindingInfo.binding         = binding;
    bindingInfo.descriptorCount = 1;
    bindingInfo.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.stageFlags      = shaderStages;

    return bindingInfo;
}

void SceneDescriptorSetInstance::allocateDescriptorSets() {
    descriptorSets.emptyInit(vulkanState, Config::MaxSceneObservers);
    allocHandle = vulkanState.descriptorPool.allocate(
        setLayout, descriptorSets.data(), descriptorSets.totalSize());
}

} // namespace ds
} // namespace rc
} // namespace bl
