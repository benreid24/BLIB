#ifndef BLIB_RENDER_DESCRIPTORS_BUFFERSHADERINPUT_HPP
#define BLIB_RENDER_DESCRIPTORS_BUFFERSHADERINPUT_HPP

#include <BLIB/Engine/HeaderHelpers.hpp>
#include <BLIB/Render/Buffers/Alignment.hpp>
#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace sr
{
/**
 * @brief Lightweight shader input wrapping a buffer that can be used for uniforms or SSBOs
 *
 * @tparam TBuffer The type of buffer to create
 * @tparam DefaultCapacity Optional size to create the buffer with on init
 * @tparam DefaultAlignment The alignment to create the buffer with by default
 * @ingroup Renderer
 */
template<typename TBuffer, std::uint32_t DefaultCapacity,
         buf::Alignment DefaultAlignment = buf::Alignment::Std140>
class BufferShaderResource : public ShaderResource {
public:
    /**
     * @brief Initializes the buffer shader input
     */
    BufferShaderResource()
    : vulkanState(nullptr) {}

    /**
     * @brief Destroys the buffer
     */
    virtual ~BufferShaderResource() = default;

    /**
     * @brief Creates the buffer if DefaultCapacity is non-zero
     *
     * @param engine The gam engine instance
     */
    void init(engine::Engine& engine) override {
        vulkanState = &engine::HeaderHelpers::getVulkanState(engine);
        if constexpr (DefaultCapacity > 0) {
            buffer.create(*vulkanState, DefaultCapacity, DefaultAlignment);
        }
    }

    /**
     * @brief Does nothing, the destructor handles releasing the buffer
     */
    void cleanup() override {}

    /**
     * @brief Does nothing, derived classes or owners are responsible for transferring updated
     *        contents of the buffer
     */
    void performTransfer() override {}

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

} // namespace sr
} // namespace rc
} // namespace bl

#endif
