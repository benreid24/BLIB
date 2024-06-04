#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_BINDINGS_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_BINDINGS_HPP

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
    static_assert(std::is_base_of_v<Binding, TBindings> && ...,
                  "All bindings must derive from Binding");

public:
    /// The number of contained bindings
    using NBindings = sizeof(TBindings...);

    /**
     * @brief Called once by the descriptor set instance during creation
     *
     * @param engine The game engine instance
     * @storageCache Descriptor component storage cache for ECS backed data
     */
    void init(engine::Engine& engine, DescriptorComponentStorageCache& storageCache);

    /**
     * @brief Fetches the payload of the given type from one of the contained bindings
     *
     * @tparam T The binding payload to get
     * @return The contained binding payload
     */
    template<typename T>
    T& get();

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
     * @param frameIndex The index to use for PerFrame resources
     */
    void writeSet(SetWriteHelper& writer, std::uint32_t frameIndex);

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
    DescriptorSetInstance::BindMode getBindMode() const;

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

private:
    std::tuple<TBindings...> bindings;

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
template<typename T, std::size_t I>
T& Bindings<TBindings...>::getHelper() {
    if constexpr (I < sizeof...(TBindings)) {
        auto& binding  = std::get<I>(bindings);
        using TPayload = typename std::decay<decltype(binding)>::TPayload;
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
    return getTypeHelper<I + i>(index);
}

template<typename... TBindings>
void Bindings<TBindings...>::init(engine::Engine& engine,
                                  DescriptorComponentStorageCache& storageCache) {
    std::size_t index = 0;
    ((std::get<TBindings>(bindings).index = index++), ...);

    std::apply([&engine, &storageCache](
                   const auto&... binding) { (binding.init(engine, storageCache), ...); },
               bindings);
}

template<typename... TBindings>
VkDescriptorType Bindings<TBindings...>::getDescriptorType(std::uint32_t index) const {
    return getTypeHelper<0>(index);
}

template<typename... TBindings>
void Bindings<TBindings...>::onFrameStart() {
    std::apply([](const auto&... binding) { (binding.onFrameStart(), ...); }, bindings);
}

template<typename... TBindings>
void Bindings<TBindings...>::writeSet(SetWriteHelper& writer, std::uint32_t frameIndex) {
    std::apply([&writer, frameIndex](
                   const auto&... binding) { (binding.writeSet(writer, frameIndex), ...); },
               bindings);
}

template<typename... TBindings>
bool Bindings<TBindings...>::allocateObject(ecs::Entity entity, scene::Key key) {
    bool failed = false;
    std::apply([&failed, entity, key](
                   const auto&... binding) { failed || = binding.allocateObject(entity, key); },
               bindings);
    if (failed) { releaseObject(entity, key); }
    return !failed;
}

template<typename... TBindings>
void Bindings<TBindings...>::releaseObject(ecs::Entity entity, scene::Key key) {
    std::apply([entity, key](const auto&... binding) { binding.releaseObject(entity, key); },
               bindings);
}

template<typename... TBindings>
DescriptorSetInstance::BindMode Bindings<TBindings...>::getBindMode() const {
    bool bindful = false;
    std::apply(
        [&bindless](const auto&... binding) {
            bindless || = binding.getBindMode() == DescriptorSetInstance::Bindful;
        },
        bindings);
    return bindful ? DescriptorSetInstance::Bindful : DescriptorSetInstance::Bindless;
}

template<typename... TBindings>
DescriptorSetInstance::SpeedBucketSetting Bindings<TBindings...>::getSpeedMode() const {
    bool speedRequired = false;
    std::apply(
        [&speedRequired](const auto&... binding) {
            speedRequired || = binding.getSpeedMode() == DescriptorSetInstance::RebindForNewSpeed;
        },
        bindings);
    return speedRequired ? DescriptorSetInstance::RebindForNewSpeed :
                           DescriptorSetInstance::SpeedAgnostic;
}

template<typename... TBindings>
bool Bindings<TBindings...>::staticDescriptorUpdateRequired() const {
    bool required = false;
    std::apply(
        [&required](const auto&... binding) {
            required || = binding.staticDescriptorUpdateRequired();
        },
        bindings);
    return required;
}

template<typename... TBindings>
bool Bindings<TBindings...>::dynamicDescriptorUpdateRequired() const {
    bool required = false;
    std::apply(
        [&required](const auto&... binding) {
            required || = binding.dynamicDescriptorUpdateRequired();
        },
        bindings);
    return required;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
