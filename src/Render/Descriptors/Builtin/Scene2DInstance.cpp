#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <array>

namespace bl
{
namespace rc
{
namespace ds
{
Scene2DInstance::Scene2DInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout)
: vulkanState(vulkanState)
, setLayout(layout) {}

Scene2DInstance::~Scene2DInstance() {
    cameraBuffer.stopTransferringEveryFrame();
    vulkanState.descriptorPool.release(allocHandle);
    cameraBuffer.destroy();
    lighting.destroy();
}

void Scene2DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                      std::uint32_t setIndex, UpdateSpeed) const {
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(ctx.currentObserverIndex()),
                            0,
                            nullptr);
}

void Scene2DInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                    scene::Key) const {
    // n/a
}

void Scene2DInstance::releaseObject(ecs::Entity, scene::Key) {
    // n/a
}

void Scene2DInstance::init(DescriptorComponentStorageCache&) {
    // allocate memory
    cameraBuffer.create(vulkanState, Config::MaxSceneObservers);
    cameraBuffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
    lighting.create(vulkanState, 1);

    // allocate descriptors
    descriptorSets.emptyInit(vulkanState, Config::MaxSceneObservers);
    allocHandle = vulkanState.descriptorPool.allocate(
        setLayout, descriptorSets.data(), descriptorSets.totalSize());

    // create and configureWrite descriptors
    for (std::uint32_t i = 0; i < descriptorSets.size(); ++i) {
        // write descriptors
        for (std::uint32_t j = 0; j < Config::MaxConcurrentFrames; ++j) {
            VkWriteDescriptorSet setWrites[2]{};

            VkDescriptorBufferInfo cameraBufferWrite{};
            cameraBufferWrite.buffer = cameraBuffer.gpuBufferHandles().getRaw(j).getBuffer();
            cameraBufferWrite.offset =
                static_cast<VkDeviceSize>(i) * cameraBuffer.alignedUniformSize();
            cameraBufferWrite.range = cameraBuffer.alignedUniformSize();

            setWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrites[0].descriptorCount = 1;
            setWrites[0].dstBinding      = 0;
            setWrites[0].dstArrayElement = 0;
            setWrites[0].dstSet          = descriptorSets.getRaw(i, j);
            setWrites[0].pBufferInfo     = &cameraBufferWrite;
            setWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            VkDescriptorBufferInfo lightingBufferWrite{};
            lightingBufferWrite.buffer = lighting.gpuBufferHandle().getBuffer();
            lightingBufferWrite.offset = 0;
            lightingBufferWrite.range  = lighting.totalAlignedSize();

            setWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrites[1].descriptorCount = 1;
            setWrites[1].dstBinding      = 1;
            setWrites[1].dstArrayElement = 0;
            setWrites[1].dstSet          = descriptorSets.getRaw(i, j);
            setWrites[1].pBufferInfo     = &lightingBufferWrite;
            setWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
        }
    }

    // TODO - sensible defaults
    Lighting& l  = lighting[0];
    l.lightCount = 0;
    l.ambient    = glm::vec3{1.f};
    lighting.transferEveryFrame();
}

bool Scene2DInstance::allocateObject(ecs::Entity, scene::Key) {
    // n/a
    return true;
}

void Scene2DInstance::handleFrameStart() {
    // noop
}

} // namespace ds
} // namespace rc
} // namespace bl
