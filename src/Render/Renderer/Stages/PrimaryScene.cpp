#include <BLIB/Render/Renderer/Stages/PrimaryScene.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>

namespace bl
{
namespace render
{
namespace stage
{

PrimaryScene::PrimaryScene(Renderer& r)
: renderer(r)
, renderPass(
      renderer.renderPassCache().getRenderPass(Config::RenderPassIds::PrimaryScene).rawPass())
, opaqueObjects(renderer)
, transparentObjects(renderer) {
    unsigned int i = 0;
    framebuffers.init(renderer.vulkanState().swapchain, [this, &i](Framebuffer& fb) {
        fb.create(renderer.vulkanState(),
                  renderPass,
                  // TODO - always render to texture for postfx?
                  renderer.vulkanState().swapchain.swapFrameAtIndex(i));
        ++i;
    });
}

void PrimaryScene::recordRenderCommands(const AttachmentSet& target,
                                        VkCommandBuffer commandBuffer) {
    framebuffers.current().recreateIfChanged(target);

    VkClearValue clearColors[1];
    clearColors[0] = {{{0.f, 0.f, 0.f, 1.f}}};

    framebuffers.current().beginRender(commandBuffer, clearColors, std::size(clearColors));
    opaqueObjects.recordRenderCommands(commandBuffer);
    transparentObjects.recordRenderCommands(commandBuffer);
    framebuffers.current().finishRender(commandBuffer);
}

} // namespace stage
} // namespace render
} // namespace bl