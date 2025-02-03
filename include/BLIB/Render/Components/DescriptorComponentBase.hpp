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
namespace priv
{
/**
 * @brief Base class helper for DescriptorComponentBase. Provides the interface for refreshing a
 *        single payload type
 *
 * @tparam TCom The ECS component type with the descriptor data
 * @tparam TPayload The descriptor payload itself
 */
template<typename TCom, typename TPayload>
class DescriptorComponentPayloadBase {
public:
    /**
     * @brief Derived classes should implement this to perform refresh+sync logic
     *
     * @param payload The payload in the descriptor set buffer to update
     */
    virtual void refreshDescriptor(TPayload& payload) = 0;
};
} // namespace priv

/**
 * @brief Base class for ECS components that map into descriptor sets
 *
 * @tparam TCom The type of the derived class
 * @tparam TFirstPayload The first type of descriptor payload that can be populated
 * @tparam TPayloads The other types of descriptor payloads the component can populate
 * @ingroup Renderer
 */
template<typename TCom, typename TFirstPayload, typename... TPayloads>
class DescriptorComponentBase
: public priv::DescriptorComponentPayloadBase<TCom, TFirstPayload>
, public priv::DescriptorComponentPayloadBase<TCom, TPayloads>... {
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
              void* payload);

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
    template<typename TPayload = TFirstPayload>
    void refresh();

protected:
    /**
     * @brief Call this when the component is modified
     */
    void markDirty();

private:
    ds::DescriptorComponentStorageBase* descriptorSet;
    scene::Key sceneKey;
    void* payload;
    bool dirty;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom, typename TFirstPayload, typename... TPayloads>
DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::DescriptorComponentBase()
: descriptorSet(nullptr)
, dirty(0) {
    static_assert(
        std::is_base_of_v<DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>, TCom>,
        "Descriptor component must inherit DescriptorComponentBase");
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
void DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::link(
    ds::DescriptorComponentStorageBase* set, scene::Key k, void* p) {
#ifdef BLIB_DEBUG
    if (descriptorSet != nullptr && descriptorSet != set) {
        BL_LOG_ERROR << "Component is used in more than one descriptor set component module";
    }
#endif
    descriptorSet = set;
    payload       = p;
    if (k.sceneId != scene::Key::InvalidSceneId) {
        dirty    = true;
        sceneKey = k;
    }
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
inline void DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::unlink() {
    descriptorSet = nullptr;
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
constexpr bool DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::isDirty() const {
    return dirty && descriptorSet != nullptr;
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
template<typename TPayload>
void DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::refresh() {
    if (descriptorSet != nullptr) {
        descriptorSet->markObjectDirty(sceneKey);
        static_cast<priv::DescriptorComponentPayloadBase<TCom, TPayload>*>(this)->refreshDescriptor(
            *static_cast<TPayload*>(payload));
        dirty = false;
    }
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
void DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::markDirty() {
    dirty = true;
}

} // namespace rcom
} // namespace rc
} // namespace bl

#endif
