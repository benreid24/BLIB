#ifndef BLIB_RENDER_COMPONENTS_DESCRIPTORCOMPONENTBASE_HPP
#define BLIB_RENDER_COMPONENTS_DESCRIPTORCOMPONENTBASE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/ShaderResources/EntityComponentShaderResource.hpp>
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
     */
    void link(sr::EntityComponentShaderResourceBase* descriptorSet, scene::Key sceneKey);

    /**
     * @brief Unlinks the component from a scene object
     */
    void unlink();

    /**
     * @brief Refreshes and syncs the descriptor value and marks as clean
     */
    template<typename TPayload = TFirstPayload>
    void refresh(TPayload& payload);

    /**
     * @brief Returns the scene key of this entity
     */
    scene::Key getSceneKey() const { return sceneKey; }

protected:
    /**
     * @brief Call this when the component is modified
     *
     * @return True if the object was marked dirty, false if already dirty
     */
    bool markDirty();

    /**
     * @brief Returns whether the object has been marked dirty and has not been refreshed yet
     */
    bool isDirty() const;

private:
    sr::EntityComponentShaderResourceBase* descriptorSet;
    scene::Key sceneKey;
    bool dirty;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom, typename TFirstPayload, typename... TPayloads>
DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::DescriptorComponentBase()
: descriptorSet(nullptr)
, dirty(false) {
    static_assert(
        std::is_base_of_v<DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>, TCom>,
        "Descriptor component must inherit DescriptorComponentBase");
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
void DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::link(
    sr::EntityComponentShaderResourceBase* set, scene::Key k) {
#ifdef BLIB_DEBUG
    if (descriptorSet != nullptr && descriptorSet != set) {
        BL_LOG_ERROR << "Component is used in more than one descriptor set component module";
    }
#endif
    descriptorSet = set;
    if (k.sceneId != scene::Key::InvalidSceneId) {
        sceneKey = k;
        dirty    = false;
        markDirty();
    }
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
inline void DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::unlink() {
    descriptorSet = nullptr;
    dirty         = false;
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
template<typename TPayload>
void DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::refresh(TPayload& payload) {
    using PayloadBase = priv::DescriptorComponentPayloadBase<TCom, TPayload>;
    static_cast<PayloadBase*>(this)->refreshDescriptor(payload);
    dirty = false;
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
bool DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::markDirty() {
    if (!dirty && descriptorSet != nullptr) {
        dirty = true;
        descriptorSet->markObjectDirty(sceneKey, this);
        return true;
    }
    return false;
}

template<typename TCom, typename TFirstPayload, typename... TPayloads>
bool DescriptorComponentBase<TCom, TFirstPayload, TPayloads...>::isDirty() const {
    return dirty;
}

} // namespace rcom
} // namespace rc
} // namespace bl

#endif
