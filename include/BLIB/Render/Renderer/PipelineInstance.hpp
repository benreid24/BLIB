#ifndef BLIB_RENDER_RENDERER_PIPELINEINSTANCE_HPP
#define BLIB_RENDER_RENDERER_PIPELINEINSTANCE_HPP

#include <BLIB/Render/Renderer/Object.hpp>
#include <BLIB/Render/Renderer/PipelineParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

class PipelineInstance {
public:
    PipelineInstance(Renderer& renderer, std::uint32_t pipelineId, bool preserveObjectOrder);

private:
    VulkanState& vulkanState;
    const bool preserveObjectOrder;
    VkPipeline pipeline;
    std::vector<Object> objects;
};

} // namespace render
} // namespace bl

#endif
