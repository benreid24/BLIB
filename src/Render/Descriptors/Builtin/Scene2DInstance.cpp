#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <array>

namespace bl
{
namespace rc
{
namespace dsi
{
Scene2DInstance::Scene2DInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout)
: SceneDescriptorSetInstance(vulkanState, layout) {}

Scene2DInstance::~Scene2DInstance() {
    cleanup();
    lighting.destroy();
}

void Scene2DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                      std::uint32_t setIndex, UpdateSpeed) const {
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            ctx.getPipelineBindPoint(),
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

void Scene2DInstance::init(sr::ShaderResourceStore&) {
    // allocate memory
    createCameraBuffer();
    lighting.create(vulkanState, 1);

    // allocate descriptors
    allocateDescriptorSets();

    // create and configureWrite descriptors
    ds::SetWriteHelper setWriter;
    setWriter.hintWriteCount(descriptorSets.size() * cfg::Limits::MaxConcurrentFrames * 4);
    setWriter.hintBufferInfoCount(descriptorSets.size() * cfg::Limits::MaxConcurrentFrames * 4);

    for (std::uint32_t i = 0; i < cfg::Limits::MaxSceneObservers; ++i) {
        for (std::uint32_t j = 0; j < cfg::Limits::MaxConcurrentFrames; ++j) {
            const auto set = descriptorSets.getRaw(i, j);

            writeCameraDescriptor(setWriter, i, j);

            VkDescriptorBufferInfo& lightingBufferWrite = setWriter.getNewBufferInfo();
            lightingBufferWrite.buffer                  = lighting.gpuBufferHandle().getBuffer();
            lightingBufferWrite.offset                  = 0;
            lightingBufferWrite.range                   = lighting.totalAlignedSize();

            VkWriteDescriptorSet& setWrite = setWriter.getNewSetWrite(set);
            setWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrite.descriptorCount       = 1;
            setWrite.dstBinding            = 1;
            setWrite.dstArrayElement       = 0;
            setWrite.dstSet                = descriptorSets.getRaw(i, j);
            setWrite.pBufferInfo           = &lightingBufferWrite;
            setWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }
    setWriter.performWrite(vulkanState.device);

    Lighting& l  = lighting[0];
    l.lightCount = 0;
    l.ambient    = glm::vec3{1.f};
    lighting.transferEveryFrame();
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
