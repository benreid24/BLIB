#ifndef BLIB_PARTICLES_DESCRIPTORSETINSTANCE_INL
#define BLIB_PARTICLES_DESCRIPTORSETINSTANCE_INL

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Particles/DescriptorSetInstance.hpp>

namespace bl
{
namespace pcl
{
template<typename T, typename GpuT>
DescriptorSetInstance<T, GpuT>::DescriptorSetInstance(engine::Engine& engine,
                                                      VkDescriptorSetLayout layout)
: rc::ds::DescriptorSetInstance(Bindful, SpeedAgnostic)
, engine(engine)
, layout(layout) {}

template<typename T, typename GpuT>
void DescriptorSetInstance<T, GpuT>::bindForObject(rc::scene::SceneRenderContext& ctx,
                                                   VkPipelineLayout layout, std::uint32_t setIndex,
                                                   rc::scene::Key objectKey) const {
    const auto it = instances.find(objectKey.sceneId);
    if (it == instances.end()) {
        BL_LOG_ERROR << "Failed to find instance for ParticleSystem " << objectKey.sceneId;
        return;
    }

    it->second.descriptorSets.current().bind(
        ctx.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, layout, setIndex);
}

template<typename T, typename GpuT>
bool DescriptorSetInstance<T, GpuT>::allocateObject(ecs::Entity entity, rc::scene::Key key) {
    if (key.updateFreq != bl::rc::UpdateSpeed::Dynamic) {
        BL_LOG_WARN
            << "All ParticleManagers should use Dynamic scene speed to avoid key collisions";
    }
    if (!instances.try_emplace(key.sceneId, engine, layout, entity).second) {
        BL_LOG_ERROR << "Collision for ParticleManager " << entity << " with scene key "
                     << key.sceneId;
        return false;
    }
    return true;
}

template<typename T, typename GpuT>
void DescriptorSetInstance<T, GpuT>::releaseObject(ecs::Entity, rc::scene::Key key) {
    instances.erase(key.sceneId);
}

template<typename T, typename GpuT>
void DescriptorSetInstance<T, GpuT>::handleFrameStart() {
    for (auto& pair : instances) { pair.second.copyData(); }
}

template<typename T, typename GpuT>
DescriptorSetInstance<T, GpuT>::Instance::Instance(engine::Engine& engine,
                                                   VkDescriptorSetLayout layout,
                                                   bl::ecs::Entity entity)
: device(engine.renderer().vulkanState().device)
, layout(layout) {
    if constexpr (HasGlobals) {
        globals.create(engine.renderer().vulkanState(), 1);
        globals.transferEveryFrame();
    }

    storage.create(engine.renderer().vulkanState(), 128);
    globalSystemInfo.create(engine.renderer().vulkanState(), 1);
    globalSystemInfo.transferEveryFrame();
    descriptorSets.init(engine.renderer().vulkanState(),
                        [this, &engine, layout](rc::vk::DescriptorSet& set) {
                            set.init(engine.renderer().vulkanState());
                            writeDescriptorSet(set);
                        });
    link = engine.ecs().getComponent<Link<T>>(entity);
    if (!link) {
        BL_LOG_CRITICAL << "Link component not created for ParticleSystem: " << entity;
        throw std::runtime_error("Link component not created for ParticleSystem");
    }
}

template<typename T, typename GpuT>
void DescriptorSetInstance<T, GpuT>::Instance::writeDescriptorSet(rc::vk::DescriptorSet& set) {
    set.allocate(layout);

    bl::rc::vk::Buffer& buffer = descriptorSets.getOther(set, storage.gpuBufferHandles());
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer.getBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range  = buffer.getSize();

    VkWriteDescriptorSet setWrites[3]{};
    setWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[0].descriptorCount = 1;
    setWrites[0].dstBinding      = 0;
    setWrites[0].dstArrayElement = 0;
    setWrites[0].dstSet          = set.getSet();
    setWrites[0].pBufferInfo     = &bufferInfo;
    setWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    VkDescriptorBufferInfo systemInfo{};
    bl::rc::vk::Buffer& ibuf = descriptorSets.getOther(set, globalSystemInfo.gpuBufferHandles());
    systemInfo.buffer        = ibuf.getBuffer();
    systemInfo.offset        = 0;
    systemInfo.range         = ibuf.getSize();

    setWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[1].descriptorCount = 1;
    setWrites[1].dstBinding      = 1;
    setWrites[1].dstArrayElement = 0;
    setWrites[1].dstSet          = set.getSet();
    setWrites[1].pBufferInfo     = &systemInfo;
    setWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorBufferInfo globalsInfo{};
    if constexpr (HasGlobals) {
        bl::rc::vk::Buffer& gbuf = descriptorSets.getOther(set, globals.gpuBufferHandles());
        globalsInfo.buffer       = gbuf.getBuffer();
        globalsInfo.offset       = 0;
        globalsInfo.range        = gbuf.getSize();

        setWrites[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrites[2].descriptorCount = 1;
        setWrites[2].dstBinding      = 1;
        setWrites[2].dstArrayElement = 0;
        setWrites[2].dstSet          = set.getSet();
        setWrites[2].pBufferInfo     = &globalsInfo;
        setWrites[2].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    vkUpdateDescriptorSets(device, HasGlobals ? 3 : 2, setWrites, 0, nullptr);
}

template<typename T, typename GpuT>
void DescriptorSetInstance<T, GpuT>::Instance::copyData() {
    globalSystemInfo[0] = *link->systemInfo;
    if (storage.performFullCopy(link->base, link->len)) {
        descriptorSets.visit([this](bl::rc::vk::DescriptorSet& ds) { writeDescriptorSet(ds); });
    }

    if constexpr (HasGlobals) { globals[0] = *link->globals; }
}

} // namespace pcl
} // namespace bl

#endif
