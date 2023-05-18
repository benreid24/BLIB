#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetInstance.hpp>

#include <BLIB/Render/Config.hpp>
#include <array>

namespace bl
{
namespace render
{
namespace ds
{
CommonSceneDescriptorSetInstance::CommonSceneDescriptorSetInstance(vk::VulkanState& vulkanState,
                                                                   VkDescriptorSetLayout layout)
: vulkanState(vulkanState)
, setLayout(layout) {}

CommonSceneDescriptorSetInstance::~CommonSceneDescriptorSetInstance() {
    cameraBuffer.stopTransferringEveryFrame();
    std::array<VkDescriptorSet, Config::MaxConcurrentFrames * Config::MaxSceneObservers> sets;
    for (unsigned int i = 0; i < Config::MaxSceneObservers; ++i) {
        for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
            sets[i * Config::MaxConcurrentFrames + j] = descriptorSets[i].getRaw(j);
        }
    }
    vulkanState.descriptorPool.release(allocHandle, sets.data());
    cameraBuffer.destroy();
}

void CommonSceneDescriptorSetInstance::bindForPipeline(VkCommandBuffer commandBuffer,
                                                       VkPipelineLayout layout,
                                                       std::uint32_t observerIndex,
                                                       std::uint32_t setIndex) const {
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &descriptorSets[observerIndex].current(),
                            0,
                            nullptr);
}

void CommonSceneDescriptorSetInstance::bindForObject(VkCommandBuffer, VkPipelineLayout,
                                                     std::uint32_t, std::uint32_t) const {
    // n/a
}

void CommonSceneDescriptorSetInstance::releaseObject(std::uint32_t, ecs::Entity) {
    // n/a
}

void CommonSceneDescriptorSetInstance::doInit(std::uint32_t, std::uint32_t) {
    // allocate memory
    cameraBuffer.create(vulkanState, Config::MaxSceneObservers);
    cameraBuffer.configureTransferAll();
    cameraBuffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);

    // allocate descriptors
    std::array<VkDescriptorSet, Config::MaxConcurrentFrames * Config::MaxSceneObservers> sets;
    for (unsigned int i = 0; i < Config::MaxSceneObservers; ++i) {
        for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
            sets[i * Config::MaxConcurrentFrames + j] = descriptorSets[i].getRaw(j);
        }
    }
    allocHandle = vulkanState.descriptorPool.allocate(setLayout, sets.data(), sets.size());

    // create and configureWrite descriptors
    for (std::uint32_t i = 0; i < descriptorSets.size(); ++i) {
        // init descriptor set object
        descriptorSets[i].emptyInit(vulkanState);
        for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
            descriptorSets[i].getRaw(j) = sets[i * Config::MaxConcurrentFrames + j];
        }

        // write descriptors
        for (std::uint32_t j = 0; j < Config::MaxConcurrentFrames; ++j) {
            VkDescriptorBufferInfo bufferWrite{};
            bufferWrite.buffer = cameraBuffer.gpuBufferHandles().getRaw(j);
            bufferWrite.offset = static_cast<VkDeviceSize>(i) * cameraBuffer.alignedUniformSize();
            bufferWrite.range  = cameraBuffer.alignedUniformSize();

            VkWriteDescriptorSet setWrite{};
            setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrite.descriptorCount = 1;
            setWrite.dstBinding      = 0;
            setWrite.dstArrayElement = 0;
            setWrite.dstSet          = descriptorSets[i].getRaw(j);
            setWrite.pBufferInfo     = &bufferWrite;
            setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            vkUpdateDescriptorSets(vulkanState.device, 1, &setWrite, 0, nullptr);
        }
    }
}

bool CommonSceneDescriptorSetInstance::doAllocateObject(std::uint32_t, ecs::Entity, UpdateSpeed) {
    // n/a
    return true;
}

void CommonSceneDescriptorSetInstance::beginSync(bool) {
    // noop
}

} // namespace ds
} // namespace render
} // namespace bl
