#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetInstance.hpp>

#include <array>

namespace bl
{
namespace render
{
namespace ds
{
CommonSceneDescriptorSetInstance::CommonSceneDescriptorSetInstance(VulkanState& vulkanState,
                                                                   VkDescriptorSetLayout layout)
: vulkanState(vulkanState)
, setLayout(layout)
, createInfo{} {
    setBinding[0].binding         = 0;
    setBinding[0].descriptorCount = 1;
    setBinding[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setBinding[0].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = std::size(setBinding);
    createInfo.pBindings    = setBinding;
}

CommonSceneDescriptorSetInstance::~CommonSceneDescriptorSetInstance() {
    viewProjBuf.stopTransferringEveryFrame();
    for (std::uint32_t i = 0; i < descriptorSets.size(); ++i) {
        std::array<const VkDescriptorSetLayoutCreateInfo*, Config::MaxConcurrentFrames> createInfos;
        createInfos.fill(&createInfo);
        vulkanState.descriptorPool.release(descriptorHandles[i],
                                           createInfos.data(),
                                           descriptorSets[i].rawData(),
                                           Config::MaxConcurrentFrames);
    }
    viewProjBuf.destroy();
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
    viewProjBuf.create(vulkanState, observerCameras.size());
    viewProjBuf.configureTransferAll();
    viewProjBuf.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);

    // create and configureWrite descriptors
    for (std::uint32_t i = 0; i < descriptorSets.size(); ++i) {
        // init descriptor set object
        descriptorSets[i].emptyInit(vulkanState);

        // allocate descriptors
        std::array<VkDescriptorSetLayout, Config::MaxConcurrentFrames> layouts;
        layouts.fill(setLayout);
        std::array<const VkDescriptorSetLayoutCreateInfo*, Config::MaxConcurrentFrames> createInfos;
        createInfos.fill(&createInfo);
        descriptorHandles[i] = vulkanState.descriptorPool.allocate(createInfos.data(),
                                                                   layouts.data(),
                                                                   descriptorSets[i].rawData(),
                                                                   Config::MaxConcurrentFrames);
        // configureWrite descriptors
        for (std::uint32_t j = 0; j < Config::MaxConcurrentFrames; ++j) {
            VkDescriptorBufferInfo bufferWrite{};
            bufferWrite.buffer = viewProjBuf.gpuBufferHandles().getRaw(j);
            bufferWrite.offset = i * viewProjBuf.alignedUniformSize();
            bufferWrite.range  = viewProjBuf.alignedUniformSize();

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

bool CommonSceneDescriptorSetInstance::doAllocateObject(std::uint32_t, ecs::Entity,
                                                        SceneObject::UpdateSpeed) {
    // n/a
    return true;
}

void CommonSceneDescriptorSetInstance::beginSync(bool) {
    // noop
}

} // namespace ds
} // namespace render
} // namespace bl
