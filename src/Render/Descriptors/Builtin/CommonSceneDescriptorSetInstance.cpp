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
        std::array<VkDescriptorSetLayout, Config::MaxConcurrentFrames> layouts;
        layouts.fill(setLayout);
        std::array<const VkDescriptorSetLayoutCreateInfo*, Config::MaxConcurrentFrames> createInfos;
        createInfos.fill(&createInfo);
        vulkanState.descriptorPool.release(descriptorHandles[i],
                                           createInfos.data(),
                                           layouts.data(),
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

void CommonSceneDescriptorSetInstance::bindForObject(VkCommandBuffer commandBuffer,
                                                     VkPipelineLayout layout,
                                                     std::uint32_t setIndex,
                                                     std::uint32_t objectId) const {
    // n/a
}

void CommonSceneDescriptorSetInstance::releaseObject(std::uint32_t sceneId, ecs::Entity entity) {
    // n/a
}

void CommonSceneDescriptorSetInstance::doInit(std::uint32_t maxStaticObjects,
                                              std::uint32_t maxDynamicObjects) {
    // allocate memory
    viewProjBuf.create(vulkanState, observerCameras.size(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    // create and write descriptors
    for (std::uint32_t i = 0; i < descriptorSets.size(); ++i) {
        // allocate descriptors
        std::array<VkDescriptorSetLayout, Config::MaxConcurrentFrames> layouts;
        layouts.fill(setLayout);
        std::array<const VkDescriptorSetLayoutCreateInfo*, Config::MaxConcurrentFrames> createInfos;
        createInfos.fill(&createInfo);
        descriptorHandles[i] = vulkanState.descriptorPool.allocate(createInfos.data(),
                                                                   layouts.data(),
                                                                   descriptorSets[i].rawData(),
                                                                   Config::MaxConcurrentFrames);
        // write descriptors
        for (std::uint32_t j = 0; j < Config::MaxConcurrentFrames; ++j) {
            VkDescriptorBufferInfo bufferWrite{};
            bufferWrite.buffer = viewProjBuf.handles().getRaw(j);
            bufferWrite.offset = i * sizeof(glm::mat4);
            bufferWrite.range  = sizeof(glm::mat4);

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

    viewProjBuf.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
}

bool CommonSceneDescriptorSetInstance::doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                                        SceneObject::UpdateSpeed updateSpeed) {
    // n/a
    return true;
}

void CommonSceneDescriptorSetInstance::beginSync(bool) {
    viewProjBuf.write(observerCameras.data(), 0, observerCount());
}

} // namespace ds
} // namespace render
} // namespace bl
