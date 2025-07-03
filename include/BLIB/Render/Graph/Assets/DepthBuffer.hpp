#ifndef BLIB_RENDER_GRAPH_ASSETS_DEPTHBUFFER_HPP
#define BLIB_RENDER_GRAPH_ASSETS_DEPTHBUFFER_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Vulkan/Image.hpp>

namespace bl
{
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
    vk::Image& getBuffer() { return buffer; }

    /**
     * @brief Clears the depth buffer
     *
     * @param commandBuffer The command buffer to issue the clear command into
     */
    void clear(VkCommandBuffer commandBuffer);

    /**
     * @brief Call on resize before using new views. Will be called possibly out of order by the
     *        asset pool
     *
     * @param newSize The new size
     */
    virtual void onResize(glm::u32vec2 newSize) override;

private:
    bool cleared;
    vk::Image buffer;

    virtual void doCreate(engine::Engine&, Renderer&, RenderTarget*) override;
    virtual void doPrepareForInput(const rg::ExecutionContext&) override {}
    virtual void doStartOutput(const rg::ExecutionContext&) override;
    virtual void doEndOutput(const rg::ExecutionContext&) override {}
    virtual void onReset() override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
