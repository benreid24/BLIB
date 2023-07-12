#include <BLIB/Render/Descriptors/Builtin/Object2DInstance.hpp>

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

Object2DInstance::Object2DInstance(engine::Engine& engine,
                                   VkDescriptorSetLayout descriptorSetLayout)
: DescriptorSetInstance(Bindless, RebindForNewSpeed)
, registry(engine.ecs())
, vulkanState(engine.renderer().vulkanState())
, descriptorSetLayout(descriptorSetLayout) {}

Object2DInstance::~Object2DInstance() { vulkanState.descriptorPool.release(alloc); }

void Object2DInstance::init(DescriptorComponentStorageCache& storageCache) {
    // create/fetch component storage
    transforms = storageCache.getComponentStorage<t2d::Transform2D,
                                                  glm::mat4,
                                                  buf::DynamicSSBO<glm::mat4>,
                                                  buf::StaticSSBO<glm::mat4>>();
    textures   = storageCache.getComponentStorage<com::Texture,
                                                std::uint32_t,
                                                buf::StaticSSBO<std::uint32_t>,
                                                buf::StaticSSBO<std::uint32_t>>();

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

void Object2DInstance::updateStaticDescriptors() {
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
    transformWrite.dstSet                = allocatedSets[0];
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
    textureWrite.dstSet                = allocatedSets[0];
    textureWrite.pBufferInfo           = &textureBufferStaticWrite;
    textureWrite.descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // perform write
    vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
}

void Object2DInstance::updateDynamicDescriptors() {
    VkWriteDescriptorSet setWrites[2 * Config::MaxConcurrentFrames]{};
    unsigned int i = 0;

    for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
        // transform buffer configureWrite
        VkDescriptorBufferInfo transformBufferWrite{};
        transformBufferWrite.buffer =
            transforms->getDynamicBuffer().gpuBufferHandles().getRaw(j).getBuffer();
        transformBufferWrite.offset = 0;
        transformBufferWrite.range  = transforms->getDynamicBuffer().getTotalRange();

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
        textureBufferWrite.buffer = textures->getDynamicBuffer().gpuBufferHandle().getBuffer();
        textureBufferWrite.offset = 0;
        textureBufferWrite.range  = textures->getDynamicBuffer().getTotalRange();

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

void Object2DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
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

void Object2DInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                     scene::Key) const {
    // noop
}

void Object2DInstance::releaseObject(ecs::Entity entity, scene::Key key) {
    transforms->releaseObject(entity, key);
    textures->releaseObject(entity, key);
}

bool Object2DInstance::allocateObject(ecs::Entity entity, scene::Key key) {
    if (!transforms->allocateObject(entity, key)) return false;
    if (!textures->allocateObject(entity, key)) {
        transforms->releaseObject(entity, key);
        return false;
    }
    return true;
}

void Object2DInstance::handleFrameStart() {
    // handled by component cache
}

} // namespace ds
} // namespace rc
} // namespace bl
