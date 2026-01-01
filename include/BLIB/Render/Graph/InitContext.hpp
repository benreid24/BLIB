#ifndef BLIB_RENDER_GRAPH_INITCONTEXT_HPP
#define BLIB_RENDER_GRAPH_INITCONTEXT_HPP

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
namespace vk
{
struct VulkanLayer;
}
namespace sr
{
class ShaderResourceStore;
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
    vk::VulkanLayer& vulkanState;
    RenderTarget& target;
    Scene* scene;

    /// Always 0 unless initializing an asset as part of MultiAsset
    unsigned int index;

    /**
     * @brief Creates the context
     *
     * @param e The engine instance
     * @param r The renderer instance
     * @param vs The vulkan state instance
     * @param rt The render target instance
     * @param s The scene the graph is for
     */
    InitContext(engine::Engine& e, Renderer& r, vk::VulkanLayer& vs, RenderTarget& rt, Scene* s);

    /**
     * @brief Creates an init context for a specific index (for MultiAsset)
     *
     * @param ctx The base context
     * @param index The index of the asset being initialized
     */
    InitContext(const InitContext& ctx, unsigned int index);

    /**
     * @brief Helper function to select the corresponding shader resource store for the given key
     *
     * @param key The key to get the store for
     * @return The shader resource store for the given key
     */
    sr::ShaderResourceStore& getShaderResourceStore(sr::StoreKey key) const;
};
} // namespace rg
} // namespace rc
} // namespace bl

#endif
