#include <BLIB/Render/Renderer/SceneRenderContext.hpp>

namespace bl
{
namespace render
{
SceneRenderContext::SceneRenderContext(const StandardAttachmentSet& target,
                                       VkCommandBuffer commandBuffer, const glm::mat4& projView,
                                       const VkRect2D& region)
: commandBuffer(commandBuffer)
, target(target)
, projView(projView)
, renderRegion(region) {}

} // namespace render
} // namespace bl
