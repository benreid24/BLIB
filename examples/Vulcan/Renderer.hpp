#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "utils/RendererState.hpp"
#include <unordered_set>

struct VertexBufferBase;

class Renderer {
public:
    Renderer(sf::WindowBase& window);
    ~Renderer();

    void render();
    void invalidateSwapChain();

    RendererState state;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

private:
    std::unordered_set<VertexBufferBase*> vertexBuffers;

    void createRenderPass();
    void createPipeline();

    void doRender(VkRenderPassBeginInfo& renderPassInfo, RenderSwapFrame& frame);

    void registerVertexBuffer(VertexBufferBase* buf);
    void unregisterVertexBuffer(VertexBufferBase* buf);
    void cleanupVertexBuffers();

    friend struct VertexBufferBase;
};

#endif
