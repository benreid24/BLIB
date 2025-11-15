#ifndef BLIB_RENDER_GRAPH_ASSETS_DEPTHBUFFER_HPP
#define BLIB_RENDER_GRAPH_ASSETS_DEPTHBUFFER_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/ShaderResources/DepthBufferShaderResource.hpp>

namespace bl
{
namespace engine
{
class Engine;
}
namespace rc
{

namespace rgi
{
/**
 * @brief Wrapper asset around the global renderer depth buffer
 *
 * @ingroup Renderer
 */
class DepthBuffer : public rg::Asset {
public:
    /**
     * @brief Creates the depth buffer asset
     */
    DepthBuffer();

    /**
     * @brief Returns the depth buffer image
     */
    vk::Image& getBuffer() { return buffer->getBuffer(); }

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
    void setSizeMode(sri::DepthBufferShaderResource::SizeMode mode);

    /**
     * @brief Recreates the depth buffer with the given size and sample count if it does not match
     *
     * @param size The dimensions of the depth buffer
     * @param sampleCount The sample count to use for the depth buffer
     */
    void ensureValid(const glm::u32vec2& size, VkSampleCountFlagBits sampleCount);

private:
    sri::DepthBufferShaderResource* buffer;
    bool cleared;

    virtual void doCreate(const rg::InitContext& ctx) override;
    virtual void doPrepareForInput(const rg::ExecutionContext&) override {}
    virtual void doStartOutput(const rg::ExecutionContext&) override;
    virtual void doEndOutput(const rg::ExecutionContext&) override {}
    virtual void onResize(glm::u32vec2 newSize) override;
    virtual void onReset() override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
