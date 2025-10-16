#ifndef BLIB_RENDER_GRAPH_INITCONTEXT_HPP
#define BLIB_RENDER_GRAPH_INITCONTEXT_HPP

namespace bl
{
namespace engine
{
class Engine;
}
namespace rc
{
class Renderer;
class RenderTarget;
class Scene;
namespace vk
{
struct VulkanState;
}
namespace rg
{
/**
 * @brief Context struct passed to graph tasks and assets during init
 *
 * @ingroup Renderer
 */
struct InitContext {
    engine::Engine& engine;
    Renderer& renderer;
    vk::VulkanState& vulkanState;
    RenderTarget& target;
    Scene* scene;

    /**
     * @brief Creates the context
     *
     * @param e The engine instance
     * @param r The renderer instance
     * @param vs The vulkan state instance
     * @param rt The render target instance
     * @param s The scene the graph is for
     */
    InitContext(engine::Engine& e, Renderer& r, vk::VulkanState& vs, RenderTarget& rt, Scene* s)
    : engine(e)
    , renderer(r)
    , vulkanState(vs)
    , target(rt)
    , scene(s) {}
};
} // namespace rg
} // namespace rc
} // namespace bl

#endif
