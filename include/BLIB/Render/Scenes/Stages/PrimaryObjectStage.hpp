#ifndef BLIB_RENDER_RENDERER_SCENES_PRIMARYOBJECTSTAGE_HPP
#define BLIB_RENDER_RENDERER_SCENES_PRIMARYOBJECTSTAGE_HPP

#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Scenes/StageBatch.hpp>
#include <BLIB/Render/Util/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>

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
    StageBatch opaqueObjects;
    StageBatch transparentObjects;

    PrimaryObjectStage(Renderer& renderer);
    void recordRenderCommands(const SceneRenderContext& context);

    friend class Scene;
};

} // namespace scene
} // namespace render
} // namespace bl

#endif
