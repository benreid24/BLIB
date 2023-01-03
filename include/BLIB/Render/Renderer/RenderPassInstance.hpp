#ifndef BLIB_RENDER_RENDERER_RENDERPASSINSTANCE_HPP
#define BLIB_RENDER_RENDERER_RENDERPASSINSTANCE_HPP

#include <BLIB/Render/Renderer/PipelineInstance.hpp>
#include <BLIB/Render/Renderer/RenderPass.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

class RenderPassInstance {
public:
    RenderPassInstance(Renderer& renderer);

private:
    VulkanState& vulkanState;
    VkRenderPass renderPass;
    std::vector<PipelineInstance> pipelines;
};

} // namespace render
} // namespace bl

#endif
