#include <BLIB/Render/Descriptors/Builtin/Object3DInstance.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Components/Texture.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Transforms/3D.hpp>

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
, descriptorSetLayout(descriptorSetLayout)
, staticEntitySlots(Scene::DefaultObjectCapacity, ecs::InvalidEntity)
, dynamicEntitySlots(Scene::DefaultObjectCapacity, ecs::InvalidEntity) {}

Object3DInstance::~Object3DInstance() {
    vulkanState.descriptorPool.release(alloc);
    transformBufferStatic.destroy();
    textureBufferStatic.destroy();
    transformBufferDynamic.destroy();
    textureBufferDynamic.destroy();
}

void Object3DInstance::init() {
    // allocate memory
    transformBufferStatic.create(vulkanState, Scene::DefaultObjectCapacity);
    textureBufferStatic.create(vulkanState, Scene::DefaultObjectCapacity);
    transformBufferDynamic.create(vulkanState, Scene::DefaultObjectCapacity);
    textureBufferDynamic.create(vulkanState, Scene::DefaultObjectCapacity);

    // allocate descriptor sets
    dynamicDescriptorSets.emptyInit(vulkanState);
    alloc = vulkanState.descriptorPool.allocate(
        descriptorSetLayout, allocatedSets, std::size(allocatedSets));
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
        dynamicDescriptorSets.getRaw(i) = allocatedSets[i + 1];
    }

    // configureWrite descriptor sets
    updateStaticDescriptors();
    updateDynamicDescriptors();
}

void Object3DInstance::updateStaticDescriptors() {
    VkWriteDescriptorSet setWrites[2]{};

    // transform buffer configureWrite
    VkDescriptorBufferInfo transformBufferStaticWrite{};
    transformBufferStaticWrite.buffer = transformBufferStatic.gpuBufferHandle().getBuffer();
    transformBufferStaticWrite.offset = 0;
    transformBufferStaticWrite.range  = transformBufferStatic.getTotalRange();

    VkWriteDescriptorSet& transformWrite = setWrites[0];
    transformWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    transformWrite.descriptorCount       = 1;
    transformWrite.dstBinding            = 0;
    transformWrite.dstArrayElement       = 0;
    transformWrite.dstSet                = allocatedSets[0];
    transformWrite.pBufferInfo           = &transformBufferStaticWrite;
    transformWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // texture buffer configureWrite
    VkDescriptorBufferInfo textureBufferStaticWrite{};
    textureBufferStaticWrite.buffer = textureBufferStatic.gpuBufferHandle().getBuffer();
    textureBufferStaticWrite.offset = 0;
    textureBufferStaticWrite.range  = textureBufferStatic.getTotalRange();

    VkWriteDescriptorSet& textureWrite = setWrites[1];
    textureWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    textureWrite.descriptorCount       = 1;
    textureWrite.dstBinding            = 1;
    textureWrite.dstArrayElement       = 0;
    textureWrite.dstSet                = allocatedSets[0];
    textureWrite.pBufferInfo           = &textureBufferStaticWrite;
    textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // perform write
    vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
}

void Object3DInstance::updateDynamicDescriptors() {
    VkWriteDescriptorSet setWrites[2 * Config::MaxConcurrentFrames]{};
    unsigned int i = 0;

    for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
        // transform buffer configureWrite
        VkDescriptorBufferInfo transformBufferWrite{};
        transformBufferWrite.buffer =
            transformBufferDynamic.gpuBufferHandles().getRaw(j).getBuffer();
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
    const auto set =
        speed == UpdateSpeed::Static ? allocatedSets[0] : dynamicDescriptorSets.current();
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &set,
                            0,
                            nullptr);
}

void Object3DInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                     scene::Key) const {
    // noop
}

void Object3DInstance::releaseObject(ecs::Entity entity, scene::Key) {
    auto components = registry.getComponentSet<t3d::Transform3D, com::Texture>(entity);
    if (components.get<t3d::Transform3D>()) { components.get<t3d::Transform3D>()->unlink(); }
    if (components.get<com::Texture>()) { components.get<com::Texture>()->unlink(); }
}

bool Object3DInstance::doAllocateObject(ecs::Entity entity, scene::Key key) {
    glm::mat4* transform   = nullptr;
    std::uint32_t* texture = nullptr;

    if (key.updateFreq == UpdateSpeed::Dynamic) {
        dynamicEntitySlots.resize(key.sceneId + 1, ecs::InvalidEntity);
        const bool grew = transformBufferDynamic.ensureSize(key.sceneId + 1);
        textureBufferDynamic.ensureSize(key.sceneId + 1);
        dynamicEntitySlots[key.sceneId] = entity;
        transform                       = &transformBufferDynamic[key.sceneId];
        texture                         = &textureBufferDynamic[key.sceneId];

        if (grew) {
            for (unsigned int i = 0; i < dynamicEntitySlots.size(); ++i) {
                if (dynamicEntitySlots[i] != ecs::InvalidEntity) {
                    doLink(dynamicEntitySlots[i],
                           {UpdateSpeed::Static, ecs::InvalidEntity},
                           &transformBufferDynamic[i],
                           &textureBufferDynamic[i]);
                }
            }
            updateDynamicDescriptors();
        }
    }
    else {
        staticEntitySlots.resize(key.sceneId + 1, ecs::InvalidEntity);
        const bool grew = transformBufferStatic.ensureSize(key.sceneId + 1);
        textureBufferStatic.ensureSize(key.sceneId + 1);
        staticEntitySlots[key.sceneId] = entity;
        transform                      = &transformBufferStatic[key.sceneId];
        texture                        = &textureBufferStatic[key.sceneId];

        if (grew) {
            for (unsigned int i = 0; i < staticEntitySlots.size(); ++i) {
                if (staticEntitySlots[i] != ecs::InvalidEntity) {
                    doLink(staticEntitySlots[i],
                           {UpdateSpeed::Static, ecs::InvalidEntity},
                           &transformBufferStatic[i],
                           &textureBufferStatic[i]);
                }
            }
            updateStaticDescriptors();
        }
    }

    return doLink(entity, key, transform, texture);
}

bool Object3DInstance::doLink(ecs::Entity entity, scene::Key key, glm::mat4* transform,
                              std::uint32_t* texture) {
    auto components = registry.getComponentSet<t3d::Transform3D, com::Texture>(entity);
    if (!components.get<t3d::Transform3D>()) {
#ifdef BLIB_DEBUG
        BL_LOG_ERROR << "Failed to create scene object for " << entity << ": Missing transform";
#endif
        return false;
    }
    components.get<t3d::Transform3D>()->link(this, key, transform);
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
        transformBufferDynamic.updateDirtyRange(dirtyDynamic.start, dirtyDynamic.size);
        // TODO - dirty-by-component so we dont re-transfer textures that havent changed
        textureBufferDynamic.transferRange(dirtyDynamic.start, dirtyDynamic.size);
    }
}

} // namespace ds
} // namespace gfx
} // namespace bl
