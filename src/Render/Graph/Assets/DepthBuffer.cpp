#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/RenderTarget.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
DepthBuffer::DepthBuffer()
: Asset(rg::AssetTags::DepthBuffer)
, cleared(false) {}

void DepthBuffer::doCreate(engine::Engine&, Renderer& r, RenderTarget* target) {
    buffer.create(r.vulkanState(),
                  vk::Image::Type::Image2D,
                  r.vulkanState().findDepthFormat(),
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                  {target->getRegionSize().x, target->getRegionSize().y},
                  VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                  VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
}

void DepthBuffer::onResize(glm::u32vec2 newSize) {
    if (isCreated()) {
        if (buffer.getSize().width != newSize.x || buffer.getSize().height != newSize.y) {
            buffer.resize(newSize, false);
        }
    }
}

void DepthBuffer::clear(VkCommandBuffer commandBuffer) {
    buffer.clearDepthAndTransition(commandBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void DepthBuffer::onReset() { cleared = false; }

void DepthBuffer::doStartOutput(const rg::ExecutionContext& ctx) {
    if (!cleared) {
        cleared = true;
        clear(ctx.commandBuffer);
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl
