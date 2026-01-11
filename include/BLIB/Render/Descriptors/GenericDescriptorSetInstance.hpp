#ifndef BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/Generic/Bindings.hpp>
#include <BLIB/Render/HeaderHelpers.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

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
     * @param renderer The renderer instance
     * @param descriptorSetLayout The descriptor set layout
     * @param bindMode The binding requirement for this set
     * @param speedMode The speed requirement for this set
     */
    GenericDescriptorSetInstance(Renderer& renderer, VkDescriptorSetLayout descriptorSetLayout,
                                 DescriptorSetInstance::EntityBindMode bindMode,
                                 DescriptorSetInstance::SpeedBucketSetting speedMode);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~GenericDescriptorSetInstance() = default;

private:
    const VkDescriptorSetLayout descriptorSetLayout;
    Renderer& renderer;
    TBindings bindings;
    vk::DescriptorSet staticDescriptorSet;
    vk::PerFrame<vk::DescriptorSet> dynamicDescriptorSets;
    SetWriteHelper setWriter;
    int staticSetsInited;
    int dynamicSetsInited;

    virtual void init(InitContext& ctx) override;
    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void releaseObject(ecs::Entity entity, scene::Key key) override;
    virtual void updateDescriptors() override;

    void updateStaticDescriptors(std::uint32_t frame);
    void updateDynamicDescriptors(std::uint32_t frame);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TBindings>
GenericDescriptorSetInstance<TBindings>::GenericDescriptorSetInstance(
    Renderer& renderer, VkDescriptorSetLayout descriptorSetLayout,
    DescriptorSetInstance::EntityBindMode bindMode,
    DescriptorSetInstance::SpeedBucketSetting speedMode)
: DescriptorSetInstance(bindMode, speedMode)
, descriptorSetLayout(descriptorSetLayout)
, renderer(renderer)
, staticSetsInited(false)
, dynamicSetsInited(cfg::Limits::MaxConcurrentFrames) {
    if (!isBindless()) {
        throw std::runtime_error("GenericDescriptorSet only supports bindless sets");
    }

    dynamicDescriptorSets.init(HeaderHelpers::getVulkanLayer(renderer),
                               [&renderer](vk::DescriptorSet& set) { set.init(renderer); });
    staticDescriptorSet.init(renderer);
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::init(InitContext& ctx) {
    bindings.init(HeaderHelpers::getVulkanLayer(renderer), ctx);

    staticSetsInited  = 0;
    dynamicSetsInited = 0;
    for (std::uint32_t i = 0; i < cfg::Limits::MaxConcurrentFrames; ++i) {
        updateStaticDescriptors(i);
        updateDynamicDescriptors(i);
    }
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
                            ctx.getPipelineBindPoint(),
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
void GenericDescriptorSetInstance<TBindings>::updateDescriptors() {
    bindings.onFrameStart();
    if (staticSetsInited > 0 || bindings.staticDescriptorUpdateRequired()) {
        --staticSetsInited;
        updateStaticDescriptors(HeaderHelpers::getVulkanLayer(renderer).currentFrameIndex());
    }
    if (dynamicSetsInited > 0 || bindings.dynamicDescriptorUpdateRequired()) {
        --dynamicSetsInited;
        updateDynamicDescriptors(HeaderHelpers::getVulkanLayer(renderer).currentFrameIndex());
    }
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::updateStaticDescriptors(std::uint32_t frame) {
    staticDescriptorSet.allocate(descriptorSetLayout);
    bindings.writeSet(setWriter, staticDescriptorSet.getSet(), UpdateSpeed::Static, frame);
    setWriter.performWrite(HeaderHelpers::getVulkanLayer(renderer).getDevice());
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::updateDynamicDescriptors(std::uint32_t i) {
    dynamicDescriptorSets.getRaw(i).allocate(descriptorSetLayout);
    bindings.writeSet(setWriter, dynamicDescriptorSets.getRaw(i).getSet(), UpdateSpeed::Dynamic, i);
    setWriter.performWrite(HeaderHelpers::getVulkanLayer(renderer).getDevice());
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
