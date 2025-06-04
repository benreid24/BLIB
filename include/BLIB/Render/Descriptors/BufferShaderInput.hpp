#ifndef BLIB_RENDER_DESCRIPTORS_BUFFERSHADERINPUT_HPP
#define BLIB_RENDER_DESCRIPTORS_BUFFERSHADERINPUT_HPP

#include <BLIB/Render/Descriptors/ShaderInput.hpp>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Lightweight shader input wrapping a buffer that can be used for uniforms or SSBOs
 *
 * @tparam TBuffer The type of buffer to create
 * @tparam DefaultCapacity Optional size to create the buffer with on init
 * @ingroup Renderer
 */
template<typename TBuffer, std::uint32_t DefaultCapacity>
class BufferShaderInput : public ShaderInput {
public:
    /**
     * @brief Initializes the buffer shader input
     */
    BufferShaderInput()
    : vulkanState(nullptr) {}

    /**
     * @brief Destroys the buffer
     */
    virtual ~BufferShaderInput() = default;

    /**
     * @brief Creates the buffer if DefaultCapacity is non-zero
     *
     * @param Unused
     * @param vs The Renderer Vulkan state
     * @param Unused
     */
    void init(engine::Engine&, vk::VulkanState& vs, const scene::MapKeyToEntityCb&) override {
        vulkanState = &vs;
        if constexpr (DefaultCapacity > 0) { buffer.create(vs, DefaultCapacity); }
    }

    /**
     * @brief Does nothing, the destructor handles releasing the buffer
     */
    void cleanup() override {}

    /**
     * @brief Does nothing, derived classes or owners are responsible for transferring updated
     *        contents of the buffer
     */
    void performGpuSync() override {}

    /**
     * @brief Does nothing. Derived classes may copy source data into the buffer here
     */
    void copyFromSource() override {}

    /**
     * @brief Always returns false
     */
    bool dynamicDescriptorUpdateRequired() const override { return false; }

    /**
     * @brief Always returns false
     */
    bool staticDescriptorUpdateRequired() const override { return false; }

    /**
     * @brief Returns the underlying buffer
     */
    TBuffer& getBuffer() { return buffer; }

protected:
    vk::VulkanState* vulkanState;
    TBuffer buffer;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
