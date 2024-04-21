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
    storage.create(engine.renderer().vulkanState(), 128);
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
void DescriptorSetInstance<T, GpuT>::Instance::writeDescriptorSet(bl::rc::vk::DescriptorSet& set) {
    set.allocate(layout);

    bl::rc::vk::Buffer& buffer = descriptorSets.getOther(set, storage.gpuBufferHandles());

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer.getBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range  = buffer.getSize();

    VkWriteDescriptorSet setWrite{};
    setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrite.descriptorCount = 1;
    setWrite.dstBinding      = 0;
    setWrite.dstArrayElement = 0;
    setWrite.dstSet          = set.getSet();
    setWrite.pBufferInfo     = &bufferInfo;
    setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    vkUpdateDescriptorSets(device, 1, &setWrite, 0, nullptr);
}

template<typename T, typename GpuT>
void DescriptorSetInstance<T, GpuT>::Instance::copyData() {
    if (storage.performFullCopy(link->base, link->len)) {
        descriptorSets.visit([this](bl::rc::vk::DescriptorSet& ds) { writeDescriptorSet(ds); });
    }
}

} // namespace pcl
} // namespace bl

#endif
