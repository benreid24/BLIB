#include <BLIB/Render/Descriptors/Builtin/Object3DInstance.hpp>

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

Object3DInstance::Object3DInstance(engine::Engine& engine,
                                   VkDescriptorSetLayout descriptorSetLayout)
: DescriptorSetInstance(Bindless, RebindForNewSpeed)
, registry(engine.ecs())
, vulkanState(engine.renderer().vulkanState())
, descriptorSetLayout(descriptorSetLayout) {}

Object3DInstance::~Object3DInstance() {
    vulkanState.descriptorPool.release(alloc);
    transformBufferStatic.destroy();
    textureBufferStatic.destroy();
}

void Object3DInstance::init() {
    // allocate memory
    transformBufferStatic.create(vulkanState, Scene::DefaultObjectCapacity);
    textureBufferStatic.create(vulkanState, Scene::DefaultObjectCapacity);

    // allocate descriptor sets
    staticDescriptorSets.emptyInit(vulkanState);
    dynamicDescriptorSets.emptyInit(vulkanState);
    VkDescriptorSet sets[2 * Config::MaxConcurrentFrames];
    alloc = vulkanState.descriptorPool.allocate(
        descriptorSetLayout, sets, Config::MaxConcurrentFrames * 2);
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
        staticDescriptorSets.getRaw(i) = sets[i];
    }
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
        dynamicDescriptorSets.getRaw(i) = sets[i + Config::MaxConcurrentFrames];
    }

    // configureWrite descriptor sets
    updateStaticDescriptors();
    updateDynamicDescriptors();
}

void Object3DInstance::updateStaticDescriptors() {
    VkWriteDescriptorSet setWrites[2 * Config::MaxConcurrentFrames] = {VkWriteDescriptorSet{},
                                                                       VkWriteDescriptorSet{}};
    unsigned int i                                                  = 0;

    for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
        // transform buffer configureWrite
        VkDescriptorBufferInfo transformBufferStaticWrite{};
        transformBufferStaticWrite.buffer = transformBufferStatic.gpuBufferHandle().getBuffer();
        transformBufferStaticWrite.offset = 0;
        transformBufferStaticWrite.range  = transformBufferStatic.getTotalRange();

        VkWriteDescriptorSet& transformWrite = setWrites[i];
        transformWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        transformWrite.descriptorCount       = 1;
        transformWrite.dstBinding            = 0;
        transformWrite.dstArrayElement       = 0;
        transformWrite.dstSet                = staticDescriptorSets.getRaw(j);
        transformWrite.pBufferInfo           = &transformBufferStaticWrite;
        transformWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        // texture buffer configureWrite
        VkDescriptorBufferInfo textureBufferStaticWrite{};
        textureBufferStaticWrite.buffer = textureBufferStatic.gpuBufferHandle().getBuffer();
        textureBufferStaticWrite.offset = 0;
        textureBufferStaticWrite.range  = textureBufferStatic.getTotalRange();

        VkWriteDescriptorSet& textureWrite = setWrites[i + 1];
        textureWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        textureWrite.descriptorCount       = 1;
        textureWrite.dstBinding            = 1;
        textureWrite.dstArrayElement       = 0;
        textureWrite.dstSet                = staticDescriptorSets.getRaw(j);
        textureWrite.pBufferInfo           = &textureBufferStaticWrite;
        textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        i += 2;
    }

    // perform write
    vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
}

void Object3DInstance::updateDynamicDescriptors() {
    VkWriteDescriptorSet setWrites[2 * Config::MaxConcurrentFrames] = {VkWriteDescriptorSet{},
                                                                       VkWriteDescriptorSet{}};
    unsigned int i                                                  = 0;

    for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
        // transform buffer configureWrite
        VkDescriptorBufferInfo transformBufferWrite{};
        transformBufferWrite.buffer = transformBufferDynamic.gpuBufferHandle().getBuffer();
        transformBufferWrite.offset = 0;
        transformBufferWrite.range  = transformBufferDynamic.getTotalRange();

        VkWriteDescriptorSet& transformWrite = setWrites[i];
        transformWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        transformWrite.descriptorCount       = 1;
        transformWrite.dstBinding            = 0;
        transformWrite.dstArrayElement       = 0;
        transformWrite.dstSet                = dynamicDescriptorSets.getRaw(j);
        transformWrite.pBufferInfo           = &transformBufferWrite;
        transformWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        // texture buffer configureWrite
        VkDescriptorBufferInfo textureBufferWrite{};
        textureBufferWrite.buffer = textureBufferDynamic.gpuBufferHandle().getBuffer();
        textureBufferWrite.offset = 0;
        textureBufferWrite.range  = textureBufferDynamic.getTotalRange();

        VkWriteDescriptorSet& textureWrite = setWrites[i + 1];
        textureWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        textureWrite.descriptorCount       = 1;
        textureWrite.dstBinding            = 1;
        textureWrite.dstArrayElement       = 0;
        textureWrite.dstSet                = dynamicDescriptorSets.getRaw(j);
        textureWrite.pBufferInfo           = &textureBufferWrite;
        textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        i += 2;
    }

    // perform write
    vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
}

void Object3DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                       std::uint32_t setIndex, UpdateSpeed speed) const {
    const auto& sets = speed == UpdateSpeed::Static ? staticDescriptorSets : dynamicDescriptorSets;
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &sets.current(),
                            0,
                            nullptr);
}

void Object3DInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                     scene::Key) const {
    // noop
}

void Object3DInstance::releaseObject(ecs::Entity entity, scene::Key) {
    auto components = registry.getComponentSet<t2d::Transform2D, com::Texture>(entity);
    if (components.get<t2d::Transform2D>()) { components.get<t2d::Transform2D>()->unlink(); }
    if (components.get<com::Texture>()) { components.get<com::Texture>()->unlink(); }
}

bool Object3DInstance::doAllocateObject(ecs::Entity entity, scene::Key key) {
    glm::mat4* transform   = nullptr;
    std::uint32_t* texture = nullptr;

    if (key.updateFreq == UpdateSpeed::Dynamic) {
        transformBufferDynamic.ensureSize(key.sceneId + 1);
        textureBufferDynamic.ensureSize(key.sceneId + 1);
        transform = &transformBufferDynamic[key.sceneId];
        texture   = &textureBufferDynamic[key.sceneId];
    }
    else {
        transformBufferStatic.ensureSize(key.sceneId + 1);
        textureBufferStatic.ensureSize(key.sceneId + 1);
        transform = &transformBufferStatic[key.sceneId];
        texture   = &textureBufferStatic[key.sceneId];
    }
    // TODO - reset ECS payload pointers on grow and update descriptor sets

    auto components = registry.getComponentSet<t2d::Transform2D, com::Texture>(entity);
    if (!components.get<t2d::Transform2D>()) {
#ifdef BLIB_DEBUG
        BL_LOG_ERROR << "Failed to create scene object for " << entity << ": Missing transform";
#endif
        return false;
    }
    components.get<t2d::Transform2D>()->link(this, key, transform);
    if (components.get<com::Texture>()) {
        components.get<com::Texture>()->link(this, key, texture);
    }
    else { *texture = res::TexturePool::ErrorTextureId; }
    return true;
}

void Object3DInstance::beginSync(DirtyRange dirtyStatic, DirtyRange dirtyDynamic) {
    if (dirtyStatic.size > 0) {
        transformBufferStatic.transferRange(dirtyStatic.start, dirtyStatic.size);
        textureBufferStatic.transferRange(dirtyStatic.start, dirtyStatic.size);
    }
    if (dirtyDynamic.size > 0) {
        transformBufferDynamic.transferRange(dirtyDynamic.start, dirtyDynamic.size);
        textureBufferDynamic.transferRange(dirtyDynamic.start, dirtyDynamic.size);
    }
}

} // namespace ds
} // namespace gfx
} // namespace bl
