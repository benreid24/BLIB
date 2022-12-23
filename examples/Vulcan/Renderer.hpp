#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "utils/RendererState.hpp"
#include <unordered_set>

class VertexBufferBase;

class Renderer {
public:
    Renderer(sf::WindowBase& window);
    ~Renderer();

    template<typename T>
    void render(const T& cb);

    void invalidateSwapChain();

    RendererState state;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

private:
    std::unordered_set<VertexBufferBase*> vertexBuffers;

    void createRenderPass();
    void createPipeline();

    template<typename T>
    void doRender(VkRenderPassBeginInfo& renderPassInfo, RenderSwapFrame& frame, const T& cb);

    void registerVertexBuffer(VertexBufferBase* buf);
    void unregisterVertexBuffer(VertexBufferBase* buf);
    void cleanupVertexBuffers();

    friend class VertexBufferBase;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void Renderer::render(const T& cb) {
    // begin render frame
    VkRenderPassBeginInfo renderPassInfo{};
    RenderSwapFrame* frame = state.beginFrame(renderPassInfo);
    if (!frame) return;

    // record render commands
    doRender(renderPassInfo, *frame, cb);

    // complete render
    state.completeFrame();
}

template<typename T>
void Renderer::doRender(VkRenderPassBeginInfo& renderPassInfo, RenderSwapFrame& frame,
                        const T& cb) {
    // setup
    VkClearValue clearColor        = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(frame.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(state.swapChainExtent.width);
    viewport.height   = static_cast<float>(state.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = state.swapChainExtent;
    vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);

    // actual draw commands
    cb(frame.commandBuffer);

    // end pass
    vkCmdEndRenderPass(frame.commandBuffer);
    if (vkEndCommandBuffer(frame.commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }
}

#endif
