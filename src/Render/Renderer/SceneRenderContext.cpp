#include <BLIB/Render/Renderer/SceneRenderContext.hpp>

namespace bl
{
namespace render
{
SceneRenderContext::SceneRenderContext(VkCommandBuffer commandBuffer, const glm::mat4& projView)
: commandBuffer(commandBuffer)
, projView(projView) {}

} // namespace render
} // namespace bl
