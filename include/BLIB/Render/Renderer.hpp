#ifndef BLIB_RENDER_RENDERER_HPP
#define BLIB_RENDER_RENDERER_HPP

#include <BLIB/Render/Observer.hpp>
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
    enum struct SplitscreenDirection { TopAndBottom, LeftAndRight };

    /**
     * @brief Updates the renderer
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Renders a frame to the window and presents it
     *
     */
    void renderFrame();

    Observer& addObserver();

    Observer& getObserver(unsigned int i = 0);

    void removeObserver(unsigned int i = 4);

    unsigned int observerCount() const;

    Scene* pushSceneToAllObservers();

    void popSceneFromAllObservers(bool popCameras = true);

    Scene* popSceneFromAllObserversNoRelease(bool popCameras = true);

    /**
     * @brief Sets the default near and far values to be used for all cameras
     * 
     * @param near The distance of the near plane
     * @param Far The distance of the far plane
    */
    void setDefaultNearAndFar(float near, float far);

    /**
     * @brief Returns the common observer for the renderer. Use this to render scenes that should be
     *        fullscreen regardless of the observer count (ie cutscenes). If the common observer has
     *        any active scenes then the other observers will not render their scenes
     *
     * @return A reference to the common observer
     */
    constexpr Observer& getCommonObserver(); // TODO - common overlay will live in here

    /**
     * @brief Sets the direction the screen will split if there is more than one observer. Default
     *        is SplitscreenDirection::TopAndBottom
     *
     * @param direction Whether to split horizontally or vertically
     */
    void setSplitscreenDirection(SplitscreenDirection direction);

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

private:
    std::mutex mutex;
    sf::WindowBase& window;
    VulkanState state;
    TexturePool textures;
    MaterialPool materials;
    RenderPassCache renderPasses;
    PipelineCache pipelines;
    ScenePool scenes;
    SplitscreenDirection splitscreenDirection;
    Observer commonObserver;
    std::vector<std::unique_ptr<Observer>> observers;
    float defaultNear, defaultFar;

    Renderer(sf::WindowBase& window);
    ~Renderer();
    void initialize();
    void cleanup();
    void processResize();

    void assignObserverRegions();

    friend class engine::Engine;
    friend class Observer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VulkanState& Renderer::vulkanState() { return state; }

inline constexpr TexturePool& Renderer::texturePool() { return textures; }

inline constexpr MaterialPool& Renderer::materialPool() { return materials; }

inline constexpr RenderPassCache& Renderer::renderPassCache() { return renderPasses; }

inline constexpr PipelineCache& Renderer::pipelineCache() { return pipelines; }

inline constexpr ScenePool& Renderer::scenePool() { return scenes; }

inline constexpr Observer& Renderer::getCommonObserver() { return commonObserver; }

inline Observer& Renderer::getObserver(unsigned int i) { return *observers[i]; }

} // namespace render
} // namespace bl

#endif
