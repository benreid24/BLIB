#ifndef BLIB_RENDER_RENDERER_HPP
#define BLIB_RENDER_RENDERER_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
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
namespace sys
{
class CameraUpdateSystem;
class RenderSystem;
} // namespace sys

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
     * @brief Adds an observer to the renderer. By default there is a single observer already. Call
     *        this for splitscreen
     *
     * @return A reference to the new observer
     */
    Observer& addObserver();

    /**
     * @brief Returns the observer at the given index
     *
     * @param i The index of the observer to get
     * @return The observer at the given index
     */
    Observer& getObserver(unsigned int i = 0);

    /**
     * @brief Removes the observer at the given index
     *
     * @param i The index of the observer to remove
     */
    void removeObserver(unsigned int i = 4);

    /**
     * @brief Returns the number of observers in the renderer, excluding the common observer
     */
    unsigned int observerCount() const;

    /**
     * @brief Allocates a single scene, pushes it to all observers, and returns it
     *
     * @param maxStaticObjectCount The maximum number of static objects in the scene
     * @param maxStaticObjectCount The maximum number of dynamic objects in the scene
     * @return A pointer to the new scene active in all observers
     */
    Scene* pushSceneToAllObservers(std::uint32_t maxStaticObjectCount,
                                   std::uint32_t maxDynamicObjectCount);

    /**
     * @brief Removes the current scene from all observers
     *
     * @param popCameras True to also remove their current cameras, false to leave them
     */
    void popSceneFromAllObservers(bool popCameras = true);

    /**
     * @brief Pops the current scene from all observers and returns it without releasing it from the
     *        scene pool. Note that only a single scene is returned, so care should be taken to only
     *        call this when all observers are using the same scene
     *
     * @param popCameras True to also pop cameras from the observers, false to leave them
     * @return The scene that was removed
     */
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

    /**
     * @brief Returns a reference to the descriptor set factory cache used by this renderer
     *
     * @return A reference to the descriptor set factory cache used by this renderer
     */
    constexpr ds::DescriptorSetFactoryCache& descriptorFactoryCache();

    /**
     * @brief Sets the color to clear the window with prior to rendering
     *
     * @param color The color to clear with
     */
    void setClearColor(const VkClearColorValue& color);

private:
    engine::Engine& engine;
    sf::WindowBase& window;
    sf::Rect<std::uint32_t> renderRegion;
    VulkanState state;
    vk::PerSwapFrame<Framebuffer> framebuffers;
    TexturePool textures;
    MaterialPool materials;
    ds::DescriptorSetFactoryCache descriptorSetFactoryCache;
    RenderPassCache renderPasses;
    PipelineCache pipelines;
    ScenePool scenes;
    SplitscreenDirection splitscreenDirection;
    Observer commonObserver;
    std::vector<std::unique_ptr<Observer>> observers;
    float defaultNear, defaultFar;
    VkClearValue clearColor;

    Renderer(engine::Engine& engine, sf::WindowBase& window);
    ~Renderer();
    void initialize();
    void cleanup();
    void processResize(const sf::Rect<std::uint32_t>& region);

    // render stages
    void updateCameras(float dt);
    void renderFrame();

    void assignObserverRegions();

    friend class engine::Engine;
    friend class Observer;
    friend class sys::CameraUpdateSystem;
    friend class sys::RenderSystem;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VulkanState& Renderer::vulkanState() { return state; }

inline constexpr TexturePool& Renderer::texturePool() { return textures; }

inline constexpr MaterialPool& Renderer::materialPool() { return materials; }

inline constexpr RenderPassCache& Renderer::renderPassCache() { return renderPasses; }

inline constexpr PipelineCache& Renderer::pipelineCache() { return pipelines; }

inline constexpr ScenePool& Renderer::scenePool() { return scenes; }

inline constexpr ds::DescriptorSetFactoryCache& Renderer::descriptorFactoryCache() {
    return descriptorSetFactoryCache;
}

inline constexpr Observer& Renderer::getCommonObserver() { return commonObserver; }

inline Observer& Renderer::getObserver(unsigned int i) { return *observers[i]; }

} // namespace render
} // namespace bl

#endif
