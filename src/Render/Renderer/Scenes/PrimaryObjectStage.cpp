#include <BLIB/Render/Renderer/Scenes/PrimaryObjectStage.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>

namespace bl
{
namespace render
{
namespace scene
{

PrimaryObjectStage::PrimaryObjectStage(Renderer& r)
: renderer(r)
, renderPass(renderer.renderPassCache()
                 .getRenderPass(Config::RenderPassIds::OffScreenSceneRender)
                 .rawPass())
, opaqueObjects(renderer)
, transparentObjects(renderer) {}

void PrimaryObjectStage::recordRenderCommands(const SceneRenderContext& context) {
    framebuffers.current().recreateIfChanged(context.target);

    VkClearValue clearColors[1];
    clearColors[0] = {{{0.f, 0.f, 0.f, 1.f}}};

    framebuffers.current().beginRender(
        context.commandBuffer, context.renderRegion, clearColors, std::size(clearColors), true);
    opaqueObjects.recordRenderCommands(context);
    transparentObjects.recordRenderCommands(context);
    framebuffers.current().finishRender(context.commandBuffer);
}

} // namespace scene
} // namespace render
} // namespace bl
