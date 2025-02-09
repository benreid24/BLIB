#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Lighting/Scene3DLighting.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <array>

namespace bl
{
namespace rc
{
namespace ds
{
Scene3DInstance::Scene3DInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout)
: vulkanState(vulkanState)
, setLayout(layout) {}

Scene3DInstance::~Scene3DInstance() {
    cameraBuffer.stopTransferringEveryFrame();
    vulkanState.descriptorPool.release(allocHandle);
    cameraBuffer.destroy();
    globalLightInfo.destroy();
    spotlights.destroy();
    pointLights.destroy();
}

void Scene3DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
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

void Scene3DInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                    scene::Key) const {
    // n/a
}

void Scene3DInstance::releaseObject(ecs::Entity, scene::Key) {
    // n/a
}

void Scene3DInstance::init(DescriptorComponentStorageCache&) {
    // allocate memory
    cameraBuffer.create(vulkanState, Config::MaxSceneObservers);
    cameraBuffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
    globalLightInfo.create(vulkanState, 1);
    spotlights.create(vulkanState, lgt::Scene3DLighting::MaxSpotLights);
    pointLights.create(vulkanState, lgt::Scene3DLighting::MaxPointLights);

    // allocate descriptors
    descriptorSets.emptyInit(vulkanState, Config::MaxSceneObservers);
    allocHandle = vulkanState.descriptorPool.allocate(
        setLayout, descriptorSets.data(), descriptorSets.totalSize());

    // create and configureWrite descriptors
    SetWriteHelper setWriter;
    setWriter.hintWriteCount(descriptorSets.size() * Config::MaxConcurrentFrames * 4);
    setWriter.hintBufferInfoCount(descriptorSets.size() * Config::MaxConcurrentFrames * 4);
    for (std::uint32_t i = 0; i < descriptorSets.size(); ++i) {
        // write descriptors
        for (std::uint32_t j = 0; j < Config::MaxConcurrentFrames; ++j) {
            const auto set = descriptorSets.getRaw(i, j);

            VkDescriptorBufferInfo& cameraBufferInfo = setWriter.getNewBufferInfo();
            cameraBufferInfo.buffer = cameraBuffer.gpuBufferHandles().getRaw(j).getBuffer();
            cameraBufferInfo.offset =
                static_cast<VkDeviceSize>(i) * cameraBuffer.alignedUniformSize();
            cameraBufferInfo.range = cameraBuffer.alignedUniformSize();

            VkWriteDescriptorSet& cameraWrite = setWriter.getNewSetWrite(set);
            cameraWrite.descriptorCount       = 1;
            cameraWrite.dstBinding            = 0;
            cameraWrite.dstArrayElement       = 0;
            cameraWrite.pBufferInfo           = &cameraBufferInfo;
            cameraWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            VkDescriptorBufferInfo& lightInfoBufferInfo = setWriter.getNewBufferInfo();
            lightInfoBufferInfo.buffer = globalLightInfo.gpuBufferHandle().getBuffer();
            lightInfoBufferInfo.offset = 0;
            lightInfoBufferInfo.range  = globalLightInfo.totalAlignedSize();

            VkWriteDescriptorSet& lightInfoWrite = setWriter.getNewSetWrite(set);
            lightInfoWrite.descriptorCount       = 1;
            lightInfoWrite.dstBinding            = 1;
            lightInfoWrite.dstArrayElement       = 0;
            lightInfoWrite.pBufferInfo           = &lightInfoBufferInfo;
            lightInfoWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            VkDescriptorBufferInfo& pointLightInfo = setWriter.getNewBufferInfo();
            pointLightInfo.buffer                  = pointLights.gpuBufferHandle().getBuffer();
            pointLightInfo.offset                  = 0;
            pointLightInfo.range                   = pointLights.totalAlignedSize();

            VkWriteDescriptorSet& pointLightWrite = setWriter.getNewSetWrite(set);
            pointLightWrite.descriptorCount       = 1;
            pointLightWrite.dstBinding            = 2;
            pointLightWrite.dstArrayElement       = 0;
            pointLightWrite.pBufferInfo           = &pointLightInfo;
            pointLightWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            VkDescriptorBufferInfo& spotlightInfo = setWriter.getNewBufferInfo();
            spotlightInfo.buffer                  = spotlights.gpuBufferHandle().getBuffer();
            spotlightInfo.offset                  = 0;
            spotlightInfo.range                   = spotlights.totalAlignedSize();

            VkWriteDescriptorSet& spotlightWrite = setWriter.getNewSetWrite(set);
            spotlightWrite.descriptorCount       = 1;
            spotlightWrite.dstBinding            = 3;
            spotlightWrite.dstArrayElement       = 0;
            spotlightWrite.pBufferInfo           = &spotlightInfo;
            spotlightWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }
    setWriter.performWrite(vulkanState.device);

    // populate lighting with defaults
    globalLightInfo.fill(lgt::LightingDescriptor3D());
    globalLightInfo[0].sun.color.diffuse = glm::vec3{0.75f};
    globalLightInfo[0].sun.dir           = glm::normalize(glm::vec3(2.f, 7.f, 1.f));
    globalLightInfo[0].globalAmbient     = glm::vec3(0.2f);
    spotlights.fill(lgt::SpotLight3D());
    pointLights.fill(lgt::PointLight3D());
    globalLightInfo.queueTransfer();
    spotlights.queueTransfer();
    pointLights.queueTransfer();
}

bool Scene3DInstance::allocateObject(ecs::Entity, scene::Key) {
    // n/a
    return true;
}

void Scene3DInstance::handleFrameStart() {
    // noop
}

} // namespace ds
} // namespace rc
} // namespace bl
