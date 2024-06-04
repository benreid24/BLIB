#ifndef BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERICDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/Generic/Bindings.hpp>
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
     * @param engine The game engine instance
     */
    GenericDescriptorSetInstance(engine::Engine& engine);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~GenericDescriptorSetInstance() = default;

    /**
     * @brief Access the given binding payload
     *
     * @tparam T The payload type to access from TBindings
     * @return A reference to the payload value used for rendering
     */
    template<typename T>
    T& getBindingPayload();

private:
    engine::Engine& engine;
    TBindings bindings;
    vk::DescriptorSet staticDescriptorSet;
    vk::PerFrame<vk::DescriptorSet> dynamicDescriptorSets;

    virtual void init(DescriptorComponentStorageCache& storageCache) override;
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
GenericDescriptorSetInstance<TBindings>::GenericDescriptorSetInstance(engine::Engine& engine)
: DescriptorSetInstance(bindings.getBindMode(), bindings.getSpeedMode())
, engine(engine) {
    if (!isBindless()) {
        throw std::runtime_error("GenericDescriptorSet only supports bindless sets");
    }
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::init(DescriptorComponentStorageCache& storageCache) {
    bindings.init(engine, storageCache);
    dynamicDescriptorSets.emptyInit(engine.renderer().vulkanState());
    updateStaticDescriptors();
    for (std::uint32_t i = 0; i < Config::MaxConcurrentFrames; ++i) { updateDynamicDescriptors(i); }
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::bindForPipeline(scene::SceneRenderContext& ctx,
                                                              VkPipelineLayout layout,
                                                              std::uint32_t setIndex,
                                                              UpdateSpeed updateFreq) const {
    const auto set = updateFreq == UpdateSpeed::Static ? staticDescriptorSet.getSet() :
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

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::bindForObject(scene::SceneRenderContext& ctx,
                                                            VkPipelineLayout layout,
                                                            std::uint32_t setIndex,
                                                            scene::Key objectKey) const {
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
    if (bindings.staticDescriptorUpdateRequired()) { updateStaticDescriptors(); }
    if (bindings.dynamicDescriptorUpdateRequired()) {
        updateDynamicDescriptors(engine.renderer().vulkanState().currentFrameIndex());
    }
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::updateStaticDescriptors() {
    SetWriteHelper writer(staticDescriptorSet.getSet());
    bindings.writeSet(writer, 0); // TODO - PerFrame for static?
    writer.performWrite(engine.renderer().vulkanState().device);
}

template<typename TBindings>
void GenericDescriptorSetInstance<TBindings>::updateDynamicDescriptors(std::uint32_t i) {
    SetWriteHelper writer(dynamicDescriptorSets.getRaw(i).getSet());
    bindings.writeSet(writer, i);
    writer.performWrite(engine.renderer().vulkanState().device);
}

template<typename TBindings>
template<typename T>
T& GenericDescriptorSetInstance<TBindings>::getBindingPayload() {
    return bindings.get<T>();
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
