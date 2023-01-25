#ifndef BLIB_RENDER_RENDERER_STAGES_PRIMARYSCENESTAGE_HPP
#define BLIB_RENDER_RENDERER_STAGES_PRIMARYSCENESTAGE_HPP

#include <BLIB/Render/Renderer/ObjectBatch.hpp>
#include <BLIB/Render/Util/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>

namespace bl
{
namespace render
{
class Renderer;
class Scene;

/// Collection of internal classes containing rendering code for various renderer stages
namespace stage
{
/**
 * @brief Contains rendering code and logic for the primary logic of scene rendering
 * 
 * @ingroup Renderer
*/
class PrimaryScene {
private:
    Renderer& renderer;
    VkRenderPass renderPass;
    PerSwapFrame<Framebuffer> framebuffers;
    ObjectBatch opaqueObjects;
    ObjectBatch transparentObjects;

    PrimaryScene(Renderer& renderer);
    void recordRenderCommands(const AttachmentSet& target, VkCommandBuffer commandBuffer);

    friend class Scene;
};

} // namespace stage
} // namespace render
} // namespace bl

#endif
