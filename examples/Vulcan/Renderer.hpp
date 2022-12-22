#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "utils/RendererState.hpp"

class Renderer {
public:
    Renderer(sf::WindowBase& window);
    ~Renderer();

    void render();
    void invalidateSwapChain();

private:
    RendererState state;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    void createRenderPass();
    void createPipeline();

    void doRender(VkRenderPassBeginInfo& renderPassInfo, RenderSwapFrame& frame);
};

#endif
