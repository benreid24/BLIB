#ifndef BLIB_RENDER_DESCRIPTORS_INITCONTEXT_HPP
#define BLIB_RENDER_DESCRIPTORS_INITCONTEXT_HPP

#include <BLIB/Render/ShaderResources/StoreKey.hpp>

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

    /**
     * @brief Helper function to select the corresponding shader resource store for the given key
     *
     * @param key The key to get the store for
     * @return The shader resource store for the given key
     */
    sr::ShaderResourceStore& getShaderResourceStore(sr::StoreKey key) {
        switch (key) {
        case sr::StoreKey::Observer:
            return observerShaderResources;
        case sr::StoreKey::Global:
            return globalShaderResources;
        case sr::StoreKey::Scene:
        default:
            return sceneShaderResources;
        }
    }
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
