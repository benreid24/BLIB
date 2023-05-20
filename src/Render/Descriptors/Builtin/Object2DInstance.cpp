#include <BLIB/Render/Descriptors/Builtin/Object2DInstance.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Components/Texture.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Transforms/2D.hpp>

bl::render::ds::Object2DInstance::Object2DInstance(engine::Engine& engine,
                                                   VkDescriptorSetLayout descriptorSetLayout)
: DescriptorSetInstance(true)
, registry(engine.ecs())
, vulkanState(engine.renderer().vulkanState())
, descriptorSetLayout(descriptorSetLayout) {}

bl::render::ds::Object2DInstance::~Object2DInstance() {
    vulkanState.descriptorPool.release(alloc);
    transformBuffer.destroy();
    textureBuffer.destroy();
}

void bl::render::ds::Object2DInstance::doInit(std::uint32_t maxStaticObjects,
                                              std::uint32_t maxDynamicObjects) {
    const std::uint32_t objectCount = maxStaticObjects + maxDynamicObjects;
    staticObjectCount               = maxStaticObjects;
    dynamicObjectCount              = maxDynamicObjects;

    // allocate memory
    transformBuffer.create(vulkanState, objectCount);
    textureBuffer.create(vulkanState, objectCount);

    // configure data transfers
    textureBuffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
    transformBuffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
    transformBuffer.configureTransferAll();
    textureBuffer.configureTransferAll();

    // allocate descriptor sets
    descriptorSets.emptyInit(vulkanState, objectCount);
    alloc = vulkanState.descriptorPool.allocate(
        descriptorSetLayout, descriptorSets.data(), descriptorSets.totalSize());

    // configureWrite descriptor sets
    for (unsigned int i = 0; i < objectCount; ++i) {
        for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
            VkWriteDescriptorSet setWrites[2] = {VkWriteDescriptorSet{}, VkWriteDescriptorSet{}};

            // transform buffer configureWrite
            VkDescriptorBufferInfo transformBufferWrite{};
            transformBufferWrite.buffer = transformBuffer.gpuBufferHandles().getRaw(j);
            transformBufferWrite.offset =
                static_cast<VkDeviceSize>(i) * transformBuffer.alignedUniformSize();
            transformBufferWrite.range = transformBuffer.alignedUniformSize();

            VkWriteDescriptorSet& transformWrite = setWrites[0];
            transformWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            transformWrite.descriptorCount       = 1;
            transformWrite.dstBinding            = 0;
            transformWrite.dstArrayElement       = 0;
            transformWrite.dstSet                = descriptorSets.getRaw(i, j);
            transformWrite.pBufferInfo           = &transformBufferWrite;
            transformWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            // texture buffer configureWrite
            VkDescriptorBufferInfo textureBufferWrite{};
            textureBufferWrite.buffer = textureBuffer.gpuBufferHandles().getRaw(j);
            textureBufferWrite.offset =
                static_cast<VkDeviceSize>(i) * textureBuffer.alignedUniformSize();
            textureBufferWrite.range = textureBuffer.alignedUniformSize();

            VkWriteDescriptorSet& textureWrite = setWrites[1];
            textureWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureWrite.descriptorCount       = 1;
            textureWrite.dstBinding            = 1;
            textureWrite.dstArrayElement       = 0;
            textureWrite.dstSet                = descriptorSets.getRaw(i, j);
            textureWrite.pBufferInfo           = &textureBufferWrite;
            textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            // perform write
            vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
        }
    }
}

void bl::render::ds::Object2DInstance::bindForPipeline(VkCommandBuffer, VkPipelineLayout,
                                                       std::uint32_t, std::uint32_t) const {
    // noop
}

void bl::render::ds::Object2DInstance::bindForObject(VkCommandBuffer commandBuffer,
                                                     VkPipelineLayout layout,
                                                     std::uint32_t setIndex,
                                                     std::uint32_t objectId) const {
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(objectId),
                            0,
                            nullptr);
}

void bl::render::ds::Object2DInstance::releaseObject(std::uint32_t, ecs::Entity entity) {
    auto components = registry.getComponentSet<t2d::Transform2D, com::Texture>(entity);
    if (components.get<t2d::Transform2D>()) { components.get<t2d::Transform2D>()->unlink(); }
    if (components.get<com::Texture>()) { components.get<com::Texture>()->unlink(); }
}

bool bl::render::ds::Object2DInstance::doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                                        UpdateSpeed) {
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
    else { textureBuffer[sceneId] = 0; }
    return true;
}

void bl::render::ds::Object2DInstance::beginSync(bool staticObjectsChanged) {
    if (staticObjectsChanged) {
        transformBuffer.configureTransferAll();
        textureBuffer.configureTransferAll();
    }
    else {
        // TODO - try to track used space to minimize transfer
        transformBuffer.configureTransferRange(staticObjectCount, dynamicObjectCount);
        textureBuffer.configureTransferRange(staticObjectCount, dynamicObjectCount);
    }
}
