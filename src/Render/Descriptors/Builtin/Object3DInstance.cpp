#include <BLIB/Render/Descriptors/Builtin/Object3DInstance.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace rc
{
namespace ds
{

Object3DInstance::Object3DInstance(engine::Engine& engine,
                                   VkDescriptorSetLayout descriptorSetLayout)
: DescriptorSetInstance(Bindless, RebindForNewSpeed)
, registry(engine.ecs())
, vulkanState(engine.renderer().vulkanState())
, descriptorSetLayout(descriptorSetLayout)
, staticDescriptorSet(engine.renderer().vulkanState())
, dynamicDescriptorSets(engine.renderer().vulkanState()) {
    dynamicDescriptorSets.emptyInit(vulkanState);
}

Object3DInstance::~Object3DInstance() {}

void Object3DInstance::init(DescriptorComponentStorageCache& storageCache) {
    // create/fetch storage modules
    transforms = storageCache.getComponentStorage<com::Transform3D,
                                                  glm::mat4,
                                                  buf::DynamicSSBO<glm::mat4>,
                                                  buf::StaticSSBO<glm::mat4>>();
    textures   = storageCache.getComponentStorage<com::Texture,
                                                std::uint32_t,
                                                buf::StaticSSBO<std::uint32_t>,
                                                buf::StaticSSBO<std::uint32_t>>();
}

void Object3DInstance::updateStaticDescriptors() {
    // (re)allocate a descriptor set
    staticDescriptorSet.allocate(descriptorSetLayout);

    VkWriteDescriptorSet setWrites[2]{};

    // transform buffer configureWrite
    VkDescriptorBufferInfo transformBufferStaticWrite{};
    transformBufferStaticWrite.buffer = transforms->getStaticBuffer().gpuBufferHandle().getBuffer();
    transformBufferStaticWrite.offset = 0;
    transformBufferStaticWrite.range  = transforms->getStaticBuffer().getTotalRange();

    VkWriteDescriptorSet& transformWrite = setWrites[0];
    transformWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    transformWrite.descriptorCount       = 1;
    transformWrite.dstBinding            = 0;
    transformWrite.dstArrayElement       = 0;
    transformWrite.dstSet                = staticDescriptorSet.getSet();
    transformWrite.pBufferInfo           = &transformBufferStaticWrite;
    transformWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // texture buffer configureWrite
    VkDescriptorBufferInfo textureBufferStaticWrite{};
    textureBufferStaticWrite.buffer = textures->getStaticBuffer().gpuBufferHandle().getBuffer();
    textureBufferStaticWrite.offset = 0;
    textureBufferStaticWrite.range  = textures->getStaticBuffer().getTotalRange();

    VkWriteDescriptorSet& textureWrite = setWrites[1];
    textureWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    textureWrite.descriptorCount       = 1;
    textureWrite.dstBinding            = 1;
    textureWrite.dstArrayElement       = 0;
    textureWrite.dstSet                = staticDescriptorSet.getSet();
    textureWrite.pBufferInfo           = &textureBufferStaticWrite;
    textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // perform write
    vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
}

void Object3DInstance::updateDynamicDescriptors() {
    // (re)allocate the descriptor set
    dynamicDescriptorSets.current().allocate(descriptorSetLayout);

    VkWriteDescriptorSet setWrites[2]{};

    // transform buffer configureWrite
    VkDescriptorBufferInfo transformBufferWrite{};
    transformBufferWrite.buffer =
        transforms->getDynamicBuffer().gpuBufferHandles().current().getBuffer();
    transformBufferWrite.offset = 0;
    transformBufferWrite.range  = transforms->getDynamicBuffer().getTotalRange();

    VkWriteDescriptorSet& transformWrite = setWrites[0];
    transformWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    transformWrite.descriptorCount       = 1;
    transformWrite.dstBinding            = 0;
    transformWrite.dstArrayElement       = 0;
    transformWrite.dstSet                = dynamicDescriptorSets.current().getSet();
    transformWrite.pBufferInfo           = &transformBufferWrite;
    transformWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // texture buffer configureWrite
    VkDescriptorBufferInfo textureBufferWrite{};
    textureBufferWrite.buffer = textures->getDynamicBuffer().gpuBufferHandle().getBuffer();
    textureBufferWrite.offset = 0;
    textureBufferWrite.range  = textures->getDynamicBuffer().getTotalRange();

    VkWriteDescriptorSet& textureWrite = setWrites[1];
    textureWrite.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    textureWrite.descriptorCount       = 1;
    textureWrite.dstBinding            = 1;
    textureWrite.dstArrayElement       = 0;
    textureWrite.dstSet                = dynamicDescriptorSets.current().getSet();
    textureWrite.pBufferInfo           = &textureBufferWrite;
    textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // perform write
    vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
}

void Object3DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                       std::uint32_t setIndex, UpdateSpeed speed) const {
    const auto set = speed == UpdateSpeed::Static ? staticDescriptorSet.getSet() :
                                                    dynamicDescriptorSets.current().getSet();
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

void Object3DInstance::releaseObject(ecs::Entity entity, scene::Key key) {
    transforms->releaseObject(entity, key);
    textures->releaseObject(entity, key);
}

bool Object3DInstance::allocateObject(ecs::Entity entity, scene::Key key) {
    if (!transforms->allocateObject(entity, key)) return false;
    if (!textures->allocateObject(entity, key)) {
        transforms->releaseObject(entity, key);
        return false;
    }
    return true;
}

void Object3DInstance::handleFrameStart() {
    if (transforms->dynamicDescriptorUpdateRequired() ||
        textures->dynamicDescriptorUpdateRequired()) {
        updateDynamicDescriptors();
    }
    if (transforms->staticDescriptorUpdateRequired() ||
        textures->staticDescriptorUpdateRequired()) {
        updateStaticDescriptors();
    }
}

} // namespace ds
} // namespace rc
} // namespace bl
