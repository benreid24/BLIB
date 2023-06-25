#ifndef BLIB_RENDER_COMPONENTS_DESCRIPTORCOMPONENTBASE_HPP
#define BLIB_RENDER_COMPONENTS_DESCRIPTORCOMPONENTBASE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <array>
#include <cstdint>
#include <type_traits>

namespace bl
{
namespace gfx
{
namespace vk
{
struct VulkanState;
}

namespace ds
{
class DescriptorSetInstance;
}

namespace com
{
/**
 * @brief Base class for ECS components that map into descriptor sets
 *
 * @tparam TCom The type of the derived class
 * @tparam TPayload The type that exists inside of the descriptor set buffer
 * @ingroup Renderer
 */
template<typename TCom, typename TPayload = TCom>
class DescriptorComponentBase {
public:
    /**
     * @brief Performs basic setup
     */
    DescriptorComponentBase();

    /**
     * @brief Links this component to an object in a scene
     *
     * @param vulkanState Renderer Vulkan state
     * @param descriptorSet The descriptor set to link to
     * @param sceneId The id of the object in the scene
     * @param payload Pointer to the data to manage in the descriptor set buffer
     */
    void link(vk::VulkanState& vulkanState, ds::DescriptorSetInstance* descriptorSet,
              std::uint32_t sceneId, TPayload* payload);

    /**
     * @brief Links this component to an object in a scene
     *
     * @param vulkanState Renderer Vulkan state
     * @param descriptorSet The descriptor set to link to
     * @param sceneId The id of the object in the scene
     * @param payload Pointers to the data to manage in the descriptor set buffer
     */
    void link(vk::VulkanState& vulkanState, ds::DescriptorSetInstance* descriptorSet,
              std::uint32_t sceneId,
              const std::array<TPayload*, Config::MaxConcurrentFrames>& payload);

    /**
     * @brief Unlinks the component from a scene object
     */
    void unlink();

    /**
     * @brief Returns whether or not this component is dirty and needs to be synced
     */
    constexpr bool isDirty() const;

    /**
     * @brief Refreshes and syncs the descriptor value and marks as clean
     */
    void refresh();

protected:
    /**
     * @brief Call this when the component is modified
     */
    void markDirty();

    /**
     * @brief Undefined. Derived classes should implement this to perform refresh+sync logic
     *
     * @param payload The payload in the descriptor set buffer to update
     */
    void refreshDescriptor(TPayload& payload);

private:
    ds::DescriptorSetInstance* descriptorSet;
    std::uint32_t sceneId;
    vk::PerFrame<TPayload*> payload;
    int dirty;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom, typename TPayload>
DescriptorComponentBase<TCom, TPayload>::DescriptorComponentBase()
: descriptorSet(nullptr)
, sceneId(0)
, dirty(0) {
    static_assert(std::is_base_of_v<DescriptorComponentBase<TCom, TPayload>, TCom>,
                  "Descriptor component must inherit DescriptorComponentBase");
}

template<typename TCom, typename TPayload>
void DescriptorComponentBase<TCom, TPayload>::link(vk::VulkanState& vs,
                                                   ds::DescriptorSetInstance* set,
                                                   std::uint32_t sid, TPayload* p) {
    descriptorSet = set;
    sceneId       = sid;
    payload.init(vs, [&vs, p](TPayload*& ap) { ap = p; });
    dirty = Config::MaxConcurrentFrames;
}

template<typename TCom, typename TPayload>
void DescriptorComponentBase<TCom, TPayload>::link(
    vk::VulkanState& vs, ds::DescriptorSetInstance* set, std::uint32_t sid,
    const std::array<TPayload*, Config::MaxConcurrentFrames>& p) {
    descriptorSet = set;
    sceneId       = sid;
    dirty         = Config::MaxConcurrentFrames;

    payload.emptyInit(vs);
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) { payload.getRaw(i) = p[i]; }
}

template<typename TCom, typename TPayload>
inline void DescriptorComponentBase<TCom, TPayload>::unlink() {
    descriptorSet = nullptr;
}

template<typename TCom, typename TPayload>
constexpr bool DescriptorComponentBase<TCom, TPayload>::isDirty() const {
    return dirty > 0 && descriptorSet != nullptr;
}

template<typename TCom, typename TPayload>
void DescriptorComponentBase<TCom, TPayload>::refresh() {
    static_assert(std::is_invocable<decltype(&TCom::refreshDescriptor), TCom&, TPayload&>::value,
                  "Descriptor components must provide a method void refreshDescriptor(TPayload&)");

    descriptorSet->markObjectDirty(sceneId);
    static_cast<TCom*>(this)->refreshDescriptor(*payload.current());
    dirty = dirty >> 1; // 2 -> 1 -> 0 -> 0 ...
}

template<typename TCom, typename TPayload>
void DescriptorComponentBase<TCom, TPayload>::markDirty() {
    dirty = Config::MaxConcurrentFrames;
}

} // namespace com
} // namespace gfx
} // namespace bl

#endif
