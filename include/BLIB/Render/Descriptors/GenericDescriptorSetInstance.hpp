#ifndef BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/Generic/Bindings.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
namespace priv
{
template<typename T>
struct IsBindings : std::false_type {};

template<typename... TBindings>
struct IsBindings<Bindings<TBindings...>> : std::true_type {};
} // namespace priv

/**
 * @brief Generic descriptor set comprised of arbitrary bindings
 *
 * @tparam TBindings The set of bindings to provide, in form Bindings<Type1, Type2...>
 * @ingroup Renderer
 */
template<typename TBindings>
class GenericDescriptorSetInstance : public DescriptorSetInstance {
    static_assert(priv::IsBindings<TBindings>::value,
                  "TBindings must be a specialization of the Bindings class");

public:
    /**
     * @brief Creates the descriptor set instance
     *
     * @param vulkanState Renderer Vulkan state
     * @param descriptorSetLayout The descriptor set layout
     * @param bindMode The binding requirement for this set
     * @param speedMode The speed requirement for this set
     */
    GenericDescriptorSetInstance(vk::VulkanState& vulkanState,
                                 VkDescriptorSetLayout descriptorSetLayout,
                                 DescriptorSetInstance::EntityBindMode bindMode,
                                 DescriptorSetInstance::SpeedBucketSetting speedMode);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~GenericDescriptorSetInstance() = default;

    /**
     * @brief Returns the binding object directly for the given binding type
     *
     * @tparam T The type of binding to fetch
     * @return A reference to the binding object itself
     */
    template<typename T>
    T& getBinding();

    /**
     * @brief Access the given binding payload
     *
     * @tparam T The payload type to access from TBindings
     * @return A reference to the payload value used for rendering
     */
    template<typename T>
    T& getBindingPayload();

private:
    const VkDescriptorSetLayout descriptorSetLayout;
    vk::VulkanState& vulkanState;
    TBindings bindings;
    vk::DescriptorSet staticDescriptorSet;
    vk::PerFrame<vk::DescriptorSet> dynamicDescriptorSets;
    SetWriteHelper setWriter;
    bool staticSetsInited;
    int dynamicSetsInited;

    virtual void init(ShaderInputStore& storageCache) override;
    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void releaseObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;

    void updateStaticDescriptors();
    void updateDynamicDescriptors(std::uint32_t frame);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TBindings>
GenericDescriptorSetInstance<TBindings>::GenericDescriptorSetInstance(
    vk::VulkanState& vulkanState, VkDescriptorSetLayout descriptorSetLayout,
    DescriptorSetInstance::EntityBindMode bindMode,
    DescriptorSetInstance::SpeedBucketSetting speedMode)
: DescriptorSetInstance(bindMode, speedMode)
, descriptorSetLayout(descriptorSetLayout)
, vulkanState(vulkanState)
, staticSetsInited(false)
, dynamicSetsInited(cfg::Limits::MaxConcurrentFrames) {
    if (!isBindless()) {
        throw std::runtime_error("GenericDescriptorSet only supports bindless sets");
    }

    dynamicDescriptorSets.init(vulkanState,
                               [&vulkanState](vk::DescriptorSet& set) { set.init(vulkanState); });
    staticDescriptorSet.init(vulkanState);
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::init(ShaderInputStore& storageCache) {
    bindings.init(vulkanState, storageCache);
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::bindForPipeline(scene::SceneRenderContext& ctx,
                                                              VkPipelineLayout layout,
                                                              std::uint32_t setIndex,
                                                              UpdateSpeed updateFreq) const {
    const auto set = updateFreq == UpdateSpeed::Static ? staticDescriptorSet.getSet() :
                                                         dynamicDescriptorSets.current().getSet();

    std::array<std::uint32_t, cfg::Limits::MaxDescriptorBindings> dynamicOffsets;
    const std::uint32_t dynamicCount =
        bindings.getDynamicOffsets(ctx, layout, setIndex, updateFreq, dynamicOffsets);

    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &set,
                            dynamicCount,
                            dynamicOffsets.data());
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::bindForObject(scene::SceneRenderContext&,
                                                            VkPipelineLayout, std::uint32_t,
                                                            scene::Key) const {
    // TODO - descriptor set per object?
}

template<typename TBindings>
bool GenericDescriptorSetInstance<TBindings>::allocateObject(ecs::Entity entity, scene::Key key) {
    if (isBindless()) { return bindings.allocateObject(entity, key); }
    else {
        // TODO - descriptor set per object?
        return false;
    }
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::releaseObject(ecs::Entity entity, scene::Key key) {
    if (isBindless()) { bindings.releaseObject(entity, key); }
    else {
        // TODO - descriptor set per object?
    }
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::handleFrameStart() {
    bindings.onFrameStart();
    if (!staticSetsInited || bindings.staticDescriptorUpdateRequired()) {
        staticSetsInited = true;
        updateStaticDescriptors();
    }
    if (dynamicSetsInited > 0 || bindings.dynamicDescriptorUpdateRequired()) {
        --dynamicSetsInited;
        updateDynamicDescriptors(vulkanState.currentFrameIndex());
    }
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::updateStaticDescriptors() {
    staticDescriptorSet.allocate(descriptorSetLayout);
    bindings.writeSet(setWriter, staticDescriptorSet.getSet(), UpdateSpeed::Static, 0);
    setWriter.performWrite(vulkanState.device);
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::updateDynamicDescriptors(std::uint32_t i) {
    dynamicDescriptorSets.getRaw(i).allocate(descriptorSetLayout);
    bindings.writeSet(setWriter, dynamicDescriptorSets.getRaw(i).getSet(), UpdateSpeed::Dynamic, i);
    setWriter.performWrite(vulkanState.device);
}

template<typename TBindings>
template<typename T>
T& GenericDescriptorSetInstance<TBindings>::getBinding() {
    return bindings.template getBinding<T>();
}

template<typename TBindings>
template<typename T>
T& GenericDescriptorSetInstance<TBindings>::getBindingPayload() {
    return bindings.template get<T>();
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
