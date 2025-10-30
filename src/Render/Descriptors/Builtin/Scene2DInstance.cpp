#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <array>

namespace bl
{
namespace rc
{
namespace dsi
{
Scene2DInstance::Scene2DInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, vulkanState(vulkanState)
, setLayout(layout) {}

Scene2DInstance::~Scene2DInstance() {
    vulkanState.descriptorPool.release(allocHandle, descriptorSets.rawData());
}

void Scene2DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                      std::uint32_t setIndex, UpdateSpeed) const {
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            ctx.getPipelineBindPoint(),
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(),
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

void Scene2DInstance::init(ds::InitContext& ctx) {
    lightingBuffer = ctx.sceneShaderResources.getShaderResourceWithKey(sri::Scene2DLightingKey);

    Overlay* overlay = dynamic_cast<Overlay*>(&ctx.scene);
    if (!overlay) {
        cameraBuffer = ctx.observerShaderResources.getShaderResourceWithKey(sri::CameraBufferKey);
    }
    else {
        cameraBuffer = ctx.sceneShaderResources.getShaderResourceWithKey(
            sri::CameraBufferKey, ctx.owner, *overlay);
    }

    // allocate descriptors
    descriptorSets.emptyInit(vulkanState);
    allocHandle = vulkanState.descriptorPool.allocate(
        setLayout, descriptorSets.rawData(), descriptorSets.size());

    // create and configureWrite descriptors
    ds::SetWriteHelper setWriter;
    setWriter.hintWriteCount(descriptorSets.size() * cfg::Limits::MaxConcurrentFrames * 4);
    setWriter.hintBufferInfoCount(descriptorSets.size() * cfg::Limits::MaxConcurrentFrames * 4);

    for (std::uint32_t j = 0; j < cfg::Limits::MaxConcurrentFrames; ++j) {
        const auto set = descriptorSets.getRaw(j);

        VkDescriptorBufferInfo& cameraBufferInfo = setWriter.getNewBufferInfo();
        cameraBufferInfo.buffer                  = cameraBuffer->getBuffer().getRawBuffer(j);
        cameraBufferInfo.offset                  = 0;
        cameraBufferInfo.range                   = cameraBuffer->getBuffer().getTotalAlignedSize();

        VkWriteDescriptorSet& cameraWrite = setWriter.getNewSetWrite(set);
        cameraWrite.descriptorCount       = 1;
        cameraWrite.dstBinding            = 0;
        cameraWrite.dstArrayElement       = 0;
        cameraWrite.pBufferInfo           = &cameraBufferInfo;
        cameraWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        VkDescriptorBufferInfo& lightingBufferWrite = setWriter.getNewBufferInfo();
        lightingBufferWrite.buffer = lightingBuffer->getBuffer().getCurrentFrameRawBuffer();
        lightingBufferWrite.offset = 0;
        lightingBufferWrite.range  = lightingBuffer->getBuffer().getTotalAlignedSize();

        VkWriteDescriptorSet& setWrite = setWriter.getNewSetWrite(set);
        setWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrite.descriptorCount       = 1;
        setWrite.dstBinding            = 1;
        setWrite.dstArrayElement       = 0;
        setWrite.dstSet                = descriptorSets.getRaw(j);
        setWrite.pBufferInfo           = &lightingBufferWrite;
        setWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    setWriter.performWrite(vulkanState.device);

    auto& l      = lightingBuffer->getBuffer()[0];
    l.lightCount = 0;
    l.ambient    = glm::vec3{1.f};

    cameraBuffer->getBuffer().transferEveryFrame();
    cameraBuffer->getBuffer().setCopyFullRange(true);
    lightingBuffer->getBuffer().transferEveryFrame();
    lightingBuffer->getBuffer().setCopyFullRange(true);
}

bool Scene2DInstance::allocateObject(ecs::Entity, scene::Key) {
    // n/a
    return true;
}

void Scene2DInstance::updateDescriptors() {
    // noop
}

} // namespace dsi
} // namespace rc
} // namespace bl
