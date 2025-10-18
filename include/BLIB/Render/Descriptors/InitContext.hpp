#ifndef BLIB_RENDER_DESCRIPTORS_INITCONTEXT_HPP
#define BLIB_RENDER_DESCRIPTORS_INITCONTEXT_HPP

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

namespace sr
{
class ShaderResourceStore;
}

namespace ds
{
/**
 * @brief Struct that gets passed to DescriptorSetInstances on init
 *
 * @ingroup Renderer
 */
struct InitContext {
    engine::Engine& engine;
    Renderer& renderer;
    RenderTarget& owner;
    Scene& scene;
    sr::ShaderResourceStore& globalShaderResources;
    sr::ShaderResourceStore& observerShaderResources;
    sr::ShaderResourceStore& sceneShaderResources;

    /**
     * @brief Creates a new init context
     *
     * @param engine The engine instance
     * @param renderer The renderer instance
     * @param owner The render target that owns the descriptor set
     * @param scene The scene the descriptor set is for
     * @param globalShaderResources Shader resources global to the renderer
     * @param observerShaderResources Shader resources of the owner
     * @param sceneShaderResources Shader resources of the scene
     */
    InitContext(engine::Engine& engine, Renderer& renderer, RenderTarget& owner, Scene& scene,
                sr::ShaderResourceStore& globalShaderResources,
                sr::ShaderResourceStore& observerShaderResources,
                sr::ShaderResourceStore& sceneShaderResources)
    : engine(engine)
    , renderer(renderer)
    , owner(owner)
    , scene(scene)
    , globalShaderResources(globalShaderResources)
    , observerShaderResources(observerShaderResources)
    , sceneShaderResources(sceneShaderResources) {}
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
