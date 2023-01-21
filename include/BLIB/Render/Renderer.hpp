#ifndef BLIB_RENDER_RENDERER_HPP
#define BLIB_RENDER_RENDERER_HPP

#include <BLIB/Render/Cameras.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>
#include <BLIB/Render/Resources/PipelineCache.hpp>
#include <BLIB/Render/Resources/RenderPassCache.hpp>
#include <BLIB/Render/Resources/ScenePool.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace engine
{
class Engine;
class RendererCleaner;
}

namespace render
{
/**
 * @brief Top level rendering system of the game engine
 *
 * @ingroup Renderer
 *
 */
class Renderer : private util::NonCopyable {
public:
    /**
     * @brief Updates the cameras
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Renders a frame to the window and presents it
     *
     */
    void renderFrame();

    /**
     * @brief The cameras in use by the render system
     *
     */
    constexpr Cameras& cameras();

    /**
     * @brief Returns the Vulkan state of the renderer
     *
     * @return constexpr VulkanState& The Vulkan instance state
     */
    constexpr VulkanState& vulkanState();

    /**
     * @brief Returns the texture pool of this renderer
     *
     * @return constexpr TexturePool& The texture pool of this renderer
     */
    constexpr TexturePool& texturePool();

    /**
     * @brief Returns the material pool of this renderer
     *
     * @return constexpr MaterialPool& The material pool of this renderer
     */
    constexpr MaterialPool& materialPool();

    /**
     * @brief Returns the render pass cache of this renderer
     *
     * @return constexpr RenderPassCache& The render pass cache of this renderer
     */
    constexpr RenderPassCache& renderPassCache();

    /**
     * @brief Returns the pipeline cache of this renderer
     *
     * @return constexpr PipelineCache& The pipeline cache of this renderer
     */
    constexpr PipelineCache& pipelineCache();

    /**
     * @brief Returns the scene pool of the renderer
     *
     * @return The scene pool belonging to this renderer
     */
    constexpr ScenePool& scenePool();

    Scene* testScene;

private:
    VulkanState state;
    TexturePool textures;
    MaterialPool materials;
    RenderPassCache renderPasses;
    PipelineCache pipelines;
    Cameras cameraSystem;
    ScenePool scenes;

    Renderer(sf::WindowBase& window);
    ~Renderer();
    void initialize();
    void cleanup();

    friend class engine::Engine;
    friend class engine::RendererCleaner;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VulkanState& Renderer::vulkanState() { return state; }

inline constexpr TexturePool& Renderer::texturePool() { return textures; }

inline constexpr MaterialPool& Renderer::materialPool() { return materials; }

inline constexpr RenderPassCache& Renderer::renderPassCache() { return renderPasses; }

inline constexpr PipelineCache& Renderer::pipelineCache() { return pipelines; }

inline constexpr ScenePool& Renderer::scenePool() { return scenes; }

inline constexpr Cameras& Renderer::cameras() { return cameraSystem; }

} // namespace render
} // namespace bl

#endif
