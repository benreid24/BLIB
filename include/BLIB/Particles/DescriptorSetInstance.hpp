#ifndef BLIB_PARTICLES_DESCRIPTORSETINSTANCE_HPP
#define BLIB_PARTICLES_DESCRIPTORSETINSTANCE_HPP

#include <BLIB/Particles/GlobalParticleSystemInfo.hpp>
#include <BLIB/Particles/Link.hpp>
#include <BLIB/Particles/RenderConfigMap.hpp>
#include <BLIB/Render/Buffers/FullyDynamicSSBO.hpp>
#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>
#include <type_traits>

namespace bl
{
namespace pcl
{
/**
 * @brief Descriptor set instance for the default descriptor set used by the default renderer.
 *        Provides an array of particle structs as well as a singular global object of config data
 *        to the shaders. Types must be specified in a specialization of RenderConfigMap
 *
 * @tparam T The particle type
 * @tparam GpuT The particle type to use in the GPU-visible buffers. Must be assignable from T
 * @ingroup Particles
 */
template<typename T, typename GpuT>
class DescriptorSetInstance : public rc::ds::DescriptorSetInstance {
public:
    /**
     * @brief Creates the descriptor set instance
     *
     * @param engine The game engine instance
     * @param layout The descriptor set layout to use
     */
    DescriptorSetInstance(engine::Engine& engine, VkDescriptorSetLayout layout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~DescriptorSetInstance() = default;

private:
    struct Instance {
        using TGlobalPayload             = typename RenderConfigMap<T>::GlobalShaderPayload;
        static constexpr bool HasGlobals = !std::is_same_v<TGlobalPayload, std::monostate>;

        const VkDevice device;
        const VkDescriptorSetLayout layout;
        rc::buf::FullyDynamicSSBO<GpuT> storage;
        rc::buf::UniformBuffer<TGlobalPayload> globals;
        rc::buf::UniformBuffer<priv::GlobalParticleSystemInfo> globalSystemInfo;
        rc::vk::PerFrame<rc::vk::DescriptorSet> descriptorSets;
        Link<T>* link;

        Instance(engine::Engine& engine, VkDescriptorSetLayout layout, ecs::Entity entity);
        void writeDescriptorSet(rc::vk::DescriptorSet& set);
        void copyData();
    };

    bl::engine::Engine& engine;
    const VkDescriptorSetLayout layout;
    mutable std::unordered_map<std::uint32_t, Instance> instances;

    virtual void init(rc::sr::ShaderResourceStore&) override {}
    virtual void bindForPipeline(rc::scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                 rc::UpdateSpeed) const override {}

    virtual void bindForObject(rc::scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, rc::scene::Key objectKey) const override;
    virtual bool allocateObject(ecs::Entity entity, rc::scene::Key key) override;
    virtual void releaseObject(ecs::Entity, rc::scene::Key key) override;
    virtual void handleFrameStart() override;
};

} // namespace pcl
} // namespace bl

#endif
