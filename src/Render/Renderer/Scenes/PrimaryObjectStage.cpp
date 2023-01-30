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
: opaqueObjects(r)
, transparentObjects(r) {}

void PrimaryObjectStage::recordRenderCommands(const SceneRenderContext& context) {
    opaqueObjects.recordRenderCommands(context);
    transparentObjects.recordRenderCommands(context);
}

} // namespace scene
} // namespace render
} // namespace bl
