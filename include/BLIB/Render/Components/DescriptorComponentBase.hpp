#ifndef BLIB_RENDER_COMPONENTS_DESCRIPTORCOMPONENTBASE_HPP
#define BLIB_RENDER_COMPONENTS_DESCRIPTORCOMPONENTBASE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Descriptors/DescriptorComponentStorage.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <array>
#include <cstdint>
#include <type_traits>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;
}

namespace rcom
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
     * @param descriptorSet The descriptor set module to link to
     * @param sceneKeyh The key of the object in the scene
     * @param payload Pointer to the data to manage in the descriptor set buffer
     */
    void link(ds::DescriptorComponentStorageBase* descriptorSet, scene::Key sceneKey,
              TPayload* payload);

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
    ds::DescriptorComponentStorageBase* descriptorSet;
    scene::Key sceneKey;
    TPayload* payload;
    bool dirty;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom, typename TPayload>
DescriptorComponentBase<TCom, TPayload>::DescriptorComponentBase()
: descriptorSet(nullptr)
, dirty(0) {
    static_assert(std::is_base_of_v<DescriptorComponentBase<TCom, TPayload>, TCom>,
                  "Descriptor component must inherit DescriptorComponentBase");
}

template<typename TCom, typename TPayload>
void DescriptorComponentBase<TCom, TPayload>::link(ds::DescriptorComponentStorageBase* set,
                                                   scene::Key k, TPayload* p) {
#ifdef BLIB_DEBUG
    if (descriptorSet != nullptr && descriptorSet != set) {
        BL_LOG_ERROR << "Component is used in more than one descriptor set component module";
    }
#endif
    descriptorSet = set;
    payload       = p;
    if (k.sceneId != ecs::InvalidEntity.id) {
        dirty    = true;
        sceneKey = k;
    }
}

template<typename TCom, typename TPayload>
inline void DescriptorComponentBase<TCom, TPayload>::unlink() {
    descriptorSet = nullptr;
}

template<typename TCom, typename TPayload>
constexpr bool DescriptorComponentBase<TCom, TPayload>::isDirty() const {
    return dirty && descriptorSet != nullptr;
}

template<typename TCom, typename TPayload>
void DescriptorComponentBase<TCom, TPayload>::refresh() {
    static_assert(std::is_invocable<decltype(&TCom::refreshDescriptor), TCom&, TPayload&>::value,
                  "Descriptor components must provide a method void refreshDescriptor(TPayload&)");

    descriptorSet->markObjectDirty(sceneKey);
    static_cast<TCom*>(this)->refreshDescriptor(*payload);
    dirty = false;
}

template<typename TCom, typename TPayload>
void DescriptorComponentBase<TCom, TPayload>::markDirty() {
    dirty = true;
}

} // namespace rcom
} // namespace rc
} // namespace bl

#endif
