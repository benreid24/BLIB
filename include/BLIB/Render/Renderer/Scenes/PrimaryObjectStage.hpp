#ifndef BLIB_RENDER_RENDERER_SCENES_PRIMARYOBJECTSTAGE_HPP
#define BLIB_RENDER_RENDERER_SCENES_PRIMARYOBJECTSTAGE_HPP

#include <BLIB/Render/Renderer/ObjectBatch.hpp>
#include <BLIB/Render/Util/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Render/Renderer/SceneRenderContext.hpp>

namespace bl
{
namespace render
{
class Renderer;
class Scene;

/// Collection of internal classes containing renderers for scenes
namespace scene
{
/**
 * @brief Contains rendering code and logic for the primary logic of scene rendering
 * 
 * @ingroup Renderer
*/
class PrimaryObjectStage {
private:
    Renderer& renderer;
    VkRenderPass renderPass;
    PerSwapFrame<Framebuffer> framebuffers;
    ObjectBatch opaqueObjects;
    ObjectBatch transparentObjects;

    PrimaryObjectStage(Renderer& renderer);
    void recordRenderCommands(const SceneRenderContext& context);

    friend class Scene;
};

} // namespace scene
} // namespace render
} // namespace bl

#endif
