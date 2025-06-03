#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_BINDING_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_BINDING_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
namespace ds
{
template<typename... TBindings>
class Bindings;

/**
 * @brief Base class for generic descriptor set binding providers
 *
 * @ingroup Renderer
 */
class Binding {
public:
    /// Derived classes should set this to the payload they provide
    using TPayload = void;

    /**
     * @brief Destroys the binding
     */
    virtual ~Binding() = default;

    /**
     * @brief Returns the descriptor type of this binding
     */
    VkDescriptorType getDescriptorType() const { return type; }

    /**
     * @brief Returns whether this descriptor binding is dynamic
     */
    bool isDynamic() const {
        return type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC ||
               type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    }

    /**
     * @brief Returns the binding index of this binding
     */
    std::uint32_t getBindingIndex() const { return index; }

    /**
     * @brief Should return the bind mode required by this binding
     */
    virtual DescriptorSetInstance::EntityBindMode getBindMode() const = 0;

    /**
     * @brief Should return the speed mode required by this binding
     */
    virtual DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const = 0;

    /**
     * @brief Called once after the descriptor set is created
     *
     * @param vulkanState Renderer Vulkan state
     * @storageCache Descriptor component storage cache for ECS backed data
     */
    virtual void init(vk::VulkanState& vulkanState, ShaderInputStore& storageCache) = 0;

    /**
     * @brief Called to write this binding to the given descriptor set
     *
     * @param writer The writer for the descriptor set being updated
     * @param set The descriptor set to write to
     * @param speed The speed of the set being written
     * @param frameIndex The index to use for PerFrame resources
     */
    virtual void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed speed,
                          std::uint32_t frameIndex) = 0;

    /**
     * @brief Called when a new object will be using the descriptor set
     *
     * @param entity The ECS id of the new object
     * @param key The scene key of the new object
     * @return True if the object could be allocated, false otherwise
     */
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) = 0;

    /**
     * @brief Called when an object will no longer be using the set
     *
     * @param entity The ECS id of the new object
     * @param key The scene key of the new object
     */
    virtual void releaseObject(ecs::Entity entity, scene::Key key) = 0;

    /**
     * @brief Called once at the beginning of every frame
     */
    virtual void onFrameStart() = 0;

    /**
     * @brief Should return a pointer to the provided payload
     */
    virtual void* getPayload() = 0;

    /**
     * @brief Should return whether or not the static descriptor set needs to be updated
     */
    virtual bool staticDescriptorUpdateRequired() const = 0;

    /**
     * @brief Should return whether or not the static descriptor set needs to be updated
     */
    virtual bool dynamicDescriptorUpdateRequired() const = 0;

    /**
     * @brief Returns the dynamic offset to use at pipeline bind time
     *
     * @param ctx The current scene render context
     * @param layout The current pipeline layout
     * @param setIndex The index of the descriptor set in the pipeline layout
     * @param updateFreq The current object speed queue being rendered
     * @return The bind offset to use
     */
    virtual std::uint32_t getDynamicOffsetForPipeline(scene::SceneRenderContext& ctx,
                                                      VkPipelineLayout layout,
                                                      std::uint32_t setIndex,
                                                      UpdateSpeed updateFreq) {
        (void)ctx;
        (void)layout;
        (void)setIndex;
        (void)updateFreq;
        return std::numeric_limits<std::uint32_t>::max();
    }

protected:
    /**
     * @brief Creates the binding
     *
     * @param type The type of descriptor the binding is
     */
    Binding(VkDescriptorType type)
    : type(type)
    , index(0) {}

private:
    const VkDescriptorType type;
    std::uint32_t index;

    template<typename... TBindings>
    friend class Bindings;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
