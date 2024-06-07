#ifndef BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/Generic/Bindings.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <type_traits>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Generic descriptor set factory. Takes the set of bindings and their pipeline stages
 *
 * @tparam TBindings The set bindings, should be Bindings<Type0, Type1, ...>
 * @tparam ...BindingStages Pipeline stages for each binding
 * @ingroup Renderer
 */
template<typename TBindings, VkPipelineStageFlags... BindingStages>
class GenericDescriptorSetFactory : public DescriptorSetFactory {
    static_assert(priv::IsBindings<TBindings>::value,
                  "TBindings must be a specialization of the Bindings class");
    static_assert(sizeof...(BindingStages) == TBindings::NBindings,
                  "Number of pipeline stages should match number of bindings");

public:
    /**
     * @brief Destroys the descriptor set factory
     */
    virtual ~GenericDescriptorSetFactory() = default;

    /**
     * @brief Creates the descriptor set layout
     *
     * @param engine The game engine instance
     * @param renderer The renderer instance
     */
    virtual void init(engine::Engine& engine, Renderer& renderer) override;

    /**
     * @brief Creates the corresponding GenericDescriptorSetInstance
     */
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;

    /**
     * @brief Returns the type of descriptor set that this factory makes
     */
    virtual std::type_index creates() const override;

private:
    vk::VulkanState* vulkanState;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TBindings, VkPipelineStageFlags... BindingStages>
void GenericDescriptorSetFactory<TBindings, BindingStages...>::init(engine::Engine&, Renderer& r) {
    vulkanState = &r.vulkanState();

    TBindings bindingInfo;
    bl::rc::vk::DescriptorPool::SetBindingInfo bindings;
    bindings.bindingCount = TBindings::NBindings;

    const auto setBinding = [&bindings, &bindingInfo](std::uint32_t i, VkPipelineStageFlags flags) {
        bindings.bindings[i].binding            = i;
        bindings.bindings[i].descriptorCount    = 1;
        bindings.bindings[i].descriptorType     = bindingInfo.getDescriptorType(i);
        bindings.bindings[i].pImmutableSamplers = nullptr;
        bindings.bindings[i].stageFlags         = flags;
    };

    std::uint32_t i = 0;
    (..., (setBinding(i++, BindingStages)));

    descriptorSetLayout = r.vulkanState().descriptorPool.createLayout(bindings);
}

template<typename TBindings, VkPipelineStageFlags... BindingStages>
std::unique_ptr<DescriptorSetInstance>
GenericDescriptorSetFactory<TBindings, BindingStages...>::createDescriptorSet() const {
    TBindings bindingInfo;
    return std::make_unique<GenericDescriptorSetInstance<TBindings>>(
        *vulkanState, descriptorSetLayout, bindingInfo.getBindMode(), bindingInfo.getSpeedMode());
}

template<typename TBindings, VkPipelineStageFlags... BindingStages>
std::type_index GenericDescriptorSetFactory<TBindings, BindingStages...>::creates() const {
    return typeid(GenericDescriptorSetInstance<TBindings>);
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
