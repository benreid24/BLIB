#ifndef BLIB_RENDER_SHADERRESOURCES_DEPTHBUFFERSHADERRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_DEPTHBUFFERSHADERRESOURCE_HPP

#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <BLIB/Render/Vulkan/Image.hpp>
#include <BLIB/Signals/Listener.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Shader resource for the depth buffer
 *
 * @ingroup Renderer
 */
class DepthBufferShaderResource
: public sr::ShaderResource
, public sig::Listener<event::SettingsChanged> {
public:
    /// How the depth buffer is sized
    enum SizeMode {
        /// The depth buffer is sized to the full window resolution
        FullScreen,

        /// The depth buffer is sized to the observer's render region
        Target
    };

    /**
     * @brief Creates the shader resource
     */
    DepthBufferShaderResource();

    /**
     * @brief Destroys the shader resource
     */
    virtual ~DepthBufferShaderResource() = default;

    /**
     * @brief Returns the depth buffer
     */
    vk::Image& getBuffer() { return buffer; }

    /**
     * @brief Clears the depth buffer
     *
     * @param commandBuffer The command buffer to issue the clear command into
     */
    void clear(VkCommandBuffer commandBuffer);

    /**
     * @brief Called by the final asset in order to properly size the depth buffer
     *
     * @param mode The size mode to use
     */
    void setSizeMode(SizeMode mode);

    /**
     * @brief Recreates the depth buffer with the given size and sample count if it does not match
     *
     * @param size The dimensions of the depth buffer
     * @param sampleCount The sample count to use for the depth buffer
     */
    void ensureValid(const glm::u32vec2& size, VkSampleCountFlagBits sampleCount);

private:
    engine::Engine* engine;
    RenderTarget* owner;
    SizeMode mode;
    vk::Image buffer;
    unsigned int dirtyFrames;

    virtual void init(engine::Engine& engine, RenderTarget& owner) override;
    virtual void cleanup() override;
    virtual void performTransfer() override;
    virtual void copyFromSource() override;
    virtual bool dynamicDescriptorUpdateRequired() const override;
    virtual bool staticDescriptorUpdateRequired() const override;
    virtual void process(const event::SettingsChanged& event) override;

    glm::u32vec2 getSize(const glm::u32vec2& targetSize) const;
    void createAttachment(const glm::u32vec2& size);
};

/**
 * @brief Shader resource key for the depth buffer
 *
 * @ingroup Renderer
 */
constexpr sr::Key<DepthBufferShaderResource> DepthBufferResourceKey{"__builtin_DepthBuffer"};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
