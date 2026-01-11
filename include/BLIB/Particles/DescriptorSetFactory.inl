#ifndef BLIB_PARTICLES_DESCRIPTORSETFACTORY_INL
#define BLIB_PARTICLES_DESCRIPTORSETFACTORY_INL

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Particles/DescriptorSetFactory.hpp>
#include <BLIB/Particles/DescriptorSetInstance.hpp>
#include <BLIB/Particles/RenderConfigMap.hpp>
#include <type_traits>

namespace bl
{
namespace pcl
{
template<typename T, typename GpuT>
void DescriptorSetFactory<T, GpuT>::init(bl::engine::Engine& e, bl::rc::Renderer& renderer) {
    using TGlobalPayload             = typename RenderConfigMap<T>::GlobalShaderPayload;
    static constexpr bool HasGlobals = !std::is_same_v<TGlobalPayload, std::monostate>;

    engine = &e;

    bl::rc::vk::DescriptorPool::SetBindingInfo bindings;
    bindings.bindingCount = HasGlobals ? 3 : 2;

    bindings.bindings[0].binding         = 0;
    bindings.bindings[0].descriptorCount = 1;
    bindings.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings.bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings.bindings[1].binding         = 1;
    bindings.bindings[1].descriptorCount = 1;
    bindings.bindings[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings.bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    if constexpr (HasGlobals) {
        bindings.bindings[2].binding         = 2;
        bindings.bindings[2].descriptorCount = 1;
        bindings.bindings[2].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings.bindings[2].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    descriptorSetLayout = renderer.getDescriptorPool().createLayout(bindings);
}

template<typename T, typename GpuT>
std::unique_ptr<rc::ds::DescriptorSetInstance> DescriptorSetFactory<T, GpuT>::createDescriptorSet()
    const {
    return std::make_unique<DescriptorSetInstance<T, GpuT>>(*engine, descriptorSetLayout);
}

template<typename T, typename GpuT>
std::type_index DescriptorSetFactory<T, GpuT>::creates() const {
    return typeid(DescriptorSetInstance<T, GpuT>);
}

} // namespace pcl
} // namespace bl

#endif
