#include <BLIB/Render/Descriptors/Builtin/Object2DInstance.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Components/Texture.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Transforms/2D.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{

Object2DInstance::Object2DInstance(engine::Engine& engine,
                                   VkDescriptorSetLayout descriptorSetLayout)
: DescriptorSetInstance(true)
, registry(engine.ecs())
, vulkanState(engine.renderer().vulkanState())
, descriptorSetLayout(descriptorSetLayout) {}

Object2DInstance::~Object2DInstance() {
    vulkanState.descriptorPool.release(alloc);
    transformBuffer.destroy();
    textureBuffer.destroy();
}

void Object2DInstance::doInit(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects) {
    const std::uint32_t objectCount = maxStaticObjects + maxDynamicObjects;

    // allocate memory
    transformBuffer.create(vulkanState, objectCount);
    textureBuffer.create(vulkanState, objectCount);

    // allocate descriptor sets
    descriptorSets.emptyInit(vulkanState);
    alloc = vulkanState.descriptorPool.allocate(
        descriptorSetLayout, descriptorSets.rawData(), Config::MaxConcurrentFrames);

    // configureWrite descriptor sets
    updateStaticDescriptors();
}

void Object2DInstance::updateStaticDescriptors() {
    for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
        VkWriteDescriptorSet setWrites[2] = {VkWriteDescriptorSet{}, VkWriteDescriptorSet{}};

        // transform buffer configureWrite
        VkDescriptorBufferInfo transformBufferWrite{};
        transformBufferWrite.buffer = transformBuffer.gpuBufferHandle().getBuffer();
        transformBufferWrite.offset = 0;
        transformBufferWrite.range  = transformBuffer.getTotalRange();

        VkWriteDescriptorSet& transformWrite = setWrites[0];
        transformWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        transformWrite.descriptorCount       = 1;
        transformWrite.dstBinding            = 0;
        transformWrite.dstArrayElement       = 0;
        transformWrite.dstSet                = descriptorSets.getRaw(j);
        transformWrite.pBufferInfo           = &transformBufferWrite;
        transformWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        // texture buffer configureWrite
        VkDescriptorBufferInfo textureBufferWrite{};
        textureBufferWrite.buffer = textureBuffer.gpuBufferHandle().getBuffer();
        textureBufferWrite.offset = 0;
        textureBufferWrite.range  = textureBuffer.getTotalRange();

        VkWriteDescriptorSet& textureWrite = setWrites[1];
        textureWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        textureWrite.descriptorCount       = 1;
        textureWrite.dstBinding            = 1;
        textureWrite.dstArrayElement       = 0;
        textureWrite.dstSet                = descriptorSets.getRaw(j);
        textureWrite.pBufferInfo           = &textureBufferWrite;
        textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        // perform write
        vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
    }
}

void Object2DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                       std::uint32_t setIndex) const {
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(),
                            0,
                            nullptr);
}

void Object2DInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                     std::uint32_t) const {
    // noop
}

void Object2DInstance::releaseObject(std::uint32_t, ecs::Entity entity) {
    auto components = registry.getComponentSet<t2d::Transform2D, com::Texture>(entity);
    if (components.get<t2d::Transform2D>()) { components.get<t2d::Transform2D>()->unlink(); }
    if (components.get<com::Texture>()) { components.get<com::Texture>()->unlink(); }
}

bool Object2DInstance::doAllocateObject(std::uint32_t sceneId, ecs::Entity entity, UpdateSpeed) {
    transformBuffer.ensureSize(sceneId + 1);
    textureBuffer.ensureSize(sceneId + 1);

    auto components = registry.getComponentSet<t2d::Transform2D, com::Texture>(entity);
    if (!components.get<t2d::Transform2D>()) {
#ifdef BLIB_DEBUG
        BL_LOG_ERROR << "Failed to create scene object for " << entity << ": Missing transform";
#endif
        return false;
    }
    components.get<t2d::Transform2D>()->link(this, sceneId, &transformBuffer[sceneId]);
    if (components.get<com::Texture>()) {
        components.get<com::Texture>()->link(this, sceneId, &textureBuffer[sceneId]);
    }
    else { textureBuffer[sceneId] = res::TexturePool::ErrorTextureId; }
    return true;
}

void Object2DInstance::beginSync(bool staticObjectsChanged) {
    if (staticObjectsChanged) {
        transformBuffer.transferAll();
        textureBuffer.transferAll();
    }
    else {
        // TODO - track used space to minimize transfer
        transformBuffer.transferAll();
        textureBuffer.transferAll();
    }
}

} // namespace ds
} // namespace gfx
} // namespace bl
