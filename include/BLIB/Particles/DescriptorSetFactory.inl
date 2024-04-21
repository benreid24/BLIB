#ifndef BLIB_PARTICLES_DESCRIPTORSETFACTORY_INL
#define BLIB_PARTICLES_DESCRIPTORSETFACTORY_INL

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Particles/DescriptorSetFactory.hpp>
#include <BLIB/Particles/DescriptorSetInstance.hpp>

namespace bl
{
namespace pcl
{
template<typename T, typename GpuT>
void DescriptorSetFactory<T, GpuT>::init(bl::engine::Engine& e, bl::rc::Renderer& renderer) {
    engine = &e;

    bl::rc::vk::DescriptorPool::SetBindingInfo bindings;
    bindings.bindingCount = 1;

    bindings.bindings[0].binding         = 0;
    bindings.bindings[0].descriptorCount = 1;
    bindings.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings.bindings[0].stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // TODO - specify in config?

    // TODO - binding for global UBO

    descriptorSetLayout = renderer.vulkanState().descriptorPool.createLayout(bindings);
}

template<typename T, typename GpuT>
std::unique_ptr<rc::ds::DescriptorSetInstance> DescriptorSetFactory<T, GpuT>::createDescriptorSet()
    const {
    return std::make_unique<DescriptorSetInstance<T, GpuT>>(*engine, descriptorSetLayout);
}

} // namespace pcl
} // namespace bl

#endif
