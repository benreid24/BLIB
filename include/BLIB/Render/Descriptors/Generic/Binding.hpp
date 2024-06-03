#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_BINDING_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_BINDING_HPP

#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
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
     * @brief Returns the binding index of this binding
     */
    std::uint32_t getBindingIndex() const { return index; }

    /**
     * @brief Called once after the descriptor set is created
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) = 0;

    /**
     * @brief Called to write this binding to the given descriptor set
     *
     * @param writer The writer for the descriptor set being updated
     * @param frameIndex The index to use for PerFrame resources
     */
    virtual void writeSet(SetWriteHelper& writer, std::uint32_t frameIndex) = 0;

    /**
     * @brief Called once at the beginning of every frame
     */
    virtual void onFrameStart() = 0;

    /**
     * @brief Should return a pointer to the provided payload
     */
    virtual void* getPayload() = 0;

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
