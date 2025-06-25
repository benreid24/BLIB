#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_BINDINGS_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_BINDINGS_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>
#include <tuple>
#include <type_traits>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Helper for containing a set of bindings for a generic descriptor set
 *
 * @tparam ...TBindings The set of bindings in the descriptor set
 * @ingroup Renderer
 */
template<typename... TBindings>
class Bindings {
    static_assert((std::is_base_of_v<Binding, TBindings> && ...),
                  "All bindings must derive from Binding");

public:
    /// The number of contained bindings
    static constexpr std::size_t NBindings = sizeof...(TBindings);

    /**
     * @brief Creates the bindings descriptor object
     */
    Bindings() = default;

    /**
     * @brief Called once by the descriptor set instance during creation
     *
     * @param engine Renderer Vulkan state
     * @storageCache Descriptor component storage cache for ECS backed data
     */
    void init(vk::VulkanState& vulkanState, ShaderInputStore& storageCache);

    /**
     * @brief Fetches the payload of the given type from one of the contained bindings
     *
     * @tparam T The binding payload to get
     * @return The contained binding payload
     */
    template<typename T>
    T& get();

    /**
     * @brief Returns the binding object itself for a particular binding
     *
     * @tparam T The type of binding to get
     * @return A reference to the binding object
     */
    template<typename T>
    T& getBinding();

    /**
     * @brief Returns the descriptor type for the given index
     *
     * @param index The index to get the type for
     * @return The type of binding at the given index
     */
    VkDescriptorType getDescriptorType(std::uint32_t index) const;

    /**
     * @brief Should be called once at the start of every frame
     */
    void onFrameStart();

    /**
     * @brief Called when a descriptor set needs to be written
     *
     * @param writer The set writer to use
     * @param set The descriptor set to write to
     * @param frameIndex The index to use for PerFrame resources
     */
    void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed speed,
                  std::uint32_t frameIndex);

    /**
     * @brief Called when a new object will be using the descriptor set
     *
     * @param entity The ECS id of the new object
     * @param key The scene key of the new object
     * @return True if the object could be allocated, false otherwise
     */
    bool allocateObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Called when an object will no longer be using the set
     *
     * @param entity The ECS id of the new object
     * @param key The scene key of the new object
     */
    void releaseObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Returns the bind mode for the descriptor set with these bindings
     */
    DescriptorSetInstance::EntityBindMode getBindMode() const;

    /**
     * @brief Returns the speed mode for the descriptor set with these bindings
     */
    DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const;

    /**
     * @brief Returns whether or not the static descriptor set needs to be updated
     */
    bool staticDescriptorUpdateRequired() const;

    /**
     * @brief Returns whether or not the static descriptor set needs to be updated
     */
    bool dynamicDescriptorUpdateRequired() const;

    /**
     * @brief Gets the dynamic bind offsets for the dynamic bindings in this set
     *
     * @param ctx The current scene render context
     * @param layout The current pipeline layout
     * @param setIndex The index of the descriptor set in the pipeline layout
     * @param updateFreq The current object speed queue being rendered
     * @param offsets The output dynamic bind offsets for the bindings
     * @return The number of dynamic offsets
     */
    std::uint32_t getDynamicOffsets(
        scene::SceneRenderContext& ctx, VkPipelineLayout layout, std::uint32_t setIndex,
        UpdateSpeed updateFreq,
        std::array<std::uint32_t, cfg::Limits::MaxDescriptorBindings>& offsets) const;

private:
    std::tuple<TBindings...> bindings;
    ctr::StaticVector<Binding*, NBindings> dynamicBindings;

    void addDynamicBindingHelper(Binding& binding);

    template<typename T, std::size_t I>
    T& getHelper();

    template<std::uint32_t I>
    VkDescriptorType getTypeHelper(std::uint32_t index) const;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TBindings>
template<typename T>
T& Bindings<TBindings...>::get() {
    return getHelper<T, 0>();
}

template<typename... TBindings>
template<typename T>
T& Bindings<TBindings...>::getBinding() {
    return std::get<T>(bindings);
}

template<typename... TBindings>
template<typename T, std::size_t I>
T& Bindings<TBindings...>::getHelper() {
    if constexpr (I < sizeof...(TBindings)) {
        auto& binding  = std::get<I>(bindings);
        using TBinding = std::decay_t<decltype(binding)>;
        using TPayload = typename TBinding::TPayload;
        if constexpr (std::is_same<TPayload, T>::value) {
            return *static_cast<T*>(binding.getPayload());
        }
        else { return getHelper<T, I + 1>(); }
    }
    else { static_assert(sizeof(T) == 0, "Descriptor payload not found"); }
}

template<typename... TBindings>
template<std::uint32_t I>
VkDescriptorType Bindings<TBindings...>::getTypeHelper(std::uint32_t index) const {
    if (index == I) { return std::get<I>(bindings).getDescriptorType(); }
    if constexpr (I + 1 < sizeof...(TBindings)) { return getTypeHelper<I + 1>(index); }
    else {
        // unreachable ideally
        throw std::runtime_error("Failed to find binding");
    }
}

template<typename... TBindings>
void Bindings<TBindings...>::init(vk::VulkanState& vulkanState, ShaderInputStore& storageCache) {
    std::size_t index = 0;
    ((std::get<TBindings>(bindings).index = index++), ...);
    ((std::get<TBindings>(bindings).init(vulkanState, storageCache)), ...);
    ((addDynamicBindingHelper(std::get<TBindings>(bindings))), ...);
}

template<typename... TBindings>
VkDescriptorType Bindings<TBindings...>::getDescriptorType(std::uint32_t index) const {
    return getTypeHelper<0>(index);
}

template<typename... TBindings>
void Bindings<TBindings...>::onFrameStart() {
    ((std::get<TBindings>(bindings).onFrameStart()), ...);
}

template<typename... TBindings>
void Bindings<TBindings...>::writeSet(SetWriteHelper& writer, VkDescriptorSet set,
                                      UpdateSpeed speed, std::uint32_t frameIndex) {
    ((std::get<TBindings>(bindings).writeSet(writer, set, speed, frameIndex)), ...);
}

template<typename... TBindings>
bool Bindings<TBindings...>::allocateObject(ecs::Entity entity, scene::Key key) {
    const bool success = ((std::get<TBindings>(bindings).allocateObject(entity, key)) && ...);
    if (!success) { releaseObject(entity, key); }
    return success;
}

template<typename... TBindings>
void Bindings<TBindings...>::releaseObject(ecs::Entity entity, scene::Key key) {
    ((std::get<TBindings>(bindings).releaseObject(entity, key)), ...);
}

template<typename... TBindings>
DescriptorSetInstance::EntityBindMode Bindings<TBindings...>::getBindMode() const {
    const bool bindful =
        ((std::get<TBindings>(bindings).getBindMode() == DescriptorSetInstance::Bindful) || ...);
    return bindful ? DescriptorSetInstance::Bindful : DescriptorSetInstance::Bindless;
}

template<typename... TBindings>
DescriptorSetInstance::SpeedBucketSetting Bindings<TBindings...>::getSpeedMode() const {
    const bool speedRequired = ((std::get<TBindings>(bindings).getSpeedMode() ==
                                 DescriptorSetInstance::RebindForNewSpeed) ||
                                ...);
    return speedRequired ? DescriptorSetInstance::RebindForNewSpeed :
                           DescriptorSetInstance::SpeedAgnostic;
}

template<typename... TBindings>
bool Bindings<TBindings...>::staticDescriptorUpdateRequired() const {
    return ((std::get<TBindings>(bindings).staticDescriptorUpdateRequired()) || ...);
}

template<typename... TBindings>
bool Bindings<TBindings...>::dynamicDescriptorUpdateRequired() const {
    return ((std::get<TBindings>(bindings).dynamicDescriptorUpdateRequired()) || ...);
}

template<typename... TBindings>
std::uint32_t Bindings<TBindings...>::getDynamicOffsets(
    scene::SceneRenderContext& ctx, VkPipelineLayout layout, std::uint32_t setIndex,
    UpdateSpeed updateFreq,
    std::array<std::uint32_t, cfg::Limits::MaxDescriptorBindings>& offsets) const {
    std::uint32_t i = 0;
    for (Binding* binding : dynamicBindings) {
        offsets[i] = binding->getDynamicOffsetForPipeline(ctx, layout, setIndex, updateFreq);
        ++i;
    }
    return dynamicBindings.size();
}

template<typename... TBindings>
void Bindings<TBindings...>::addDynamicBindingHelper(Binding& binding) {
    if (binding.isDynamic()) { dynamicBindings.emplace_back(&binding); }
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
